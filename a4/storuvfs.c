#include <arpa/inet.h>
#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include "disk.h"


/*
 * Based on http://bit.ly/2vniWNb
 */
void pack_current_datetime(unsigned char *entry) {
    assert(entry);

    time_t t = time(NULL);
    struct tm tm = *localtime(&t);

    unsigned short year   = tm.tm_year + 1900;
    unsigned char  month  = (unsigned char)(tm.tm_mon + 1);
    unsigned char  day    = (unsigned char)(tm.tm_mday);
    unsigned char  hour   = (unsigned char)(tm.tm_hour);
    unsigned char  minute = (unsigned char)(tm.tm_min);
    unsigned char  second = (unsigned char)(tm.tm_sec);

    year = htons(year);

    memcpy(entry, &year, 2);
    entry[2] = month;
    entry[3] = day;
    entry[4] = hour;
    entry[5] = minute;
    entry[6] = second; 
}


int next_free_block(int *FAT, int max_blocks) {
    assert(FAT != NULL);

    int i;

    for (i = 0; i < max_blocks; i++) {
        if (FAT[i] == FAT_AVAILABLE) {
            return i;
        }
    }

    return -1;
}


int main(int argc, char *argv[]) {
    superblock_entry_t sb;
    int  i;
    char *imagename  = NULL;
    char *filename   = NULL;
    char *sourcename = NULL;
    FILE *f;

    for (i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--image") == 0 && i+1 < argc) {
            imagename = argv[i+1];
            i++;
        } else if (strcmp(argv[i], "--file") == 0 && i+1 < argc) {
            filename = argv[i+1];
            i++;
        } else if (strcmp(argv[i], "--source") == 0 && i+1 < argc) {
            sourcename = argv[i+1];
            i++;
        }
    }

    if (imagename == NULL || filename == NULL || sourcename == NULL) {
        fprintf(stderr, "usage: storuvfs --image <imagename> " \
            "--file <filename in image> " \
            "--source <filename on host>\n");
        exit(1);
    }

    // Open binary
    f = fopen(imagename, "r+");
    fseek(f, 0, SEEK_SET);
    fread(&sb, sizeof(sb), 1, f);

    // Big endian
    sb.block_size = htons(sb.block_size);
    sb.num_blocks = htonl(sb.num_blocks);
    sb.fat_start = htonl(sb.fat_start);
    sb.fat_blocks = htonl(sb.fat_blocks);
    sb.dir_start = htonl(sb.dir_start);
    sb.dir_blocks = htonl(sb.dir_blocks);

    // Start at beginning of root directory
    fseek(f, sb.dir_start * sb.block_size, SEEK_SET);

    int num_enteries = sb.dir_blocks * (sb.block_size / SIZE_DIR_ENTRY);
    directory_entry_t dir;

    int found = 0;

    // Check if filename already exists
    for (i=0; i<num_enteries; i++) {
        fread(&dir, sizeof(directory_entry_t), 1, f);
        if (dir.status != DIR_ENTRY_AVAILABLE) {
            if (strncmp(dir.filename, filename, strlen(filename) +1) == 0) {
                found = 1;
                break;
            }
        }
    }

    if (found) {
        printf("file already exists in the image\n");
        exit(1);
    }

    // Check that space is avalible
    FILE *s;

    s = fopen(sourcename, "r");
    fseek(s, 0, SEEK_END);
    int size_required = ftell(s);

    int free = 0;
    int fat_data;

    fseek(f, sb.fat_start * sb.block_size, SEEK_SET);
    for (i=0; i<sb.num_blocks; i++) {
        fread(&fat_data, SIZE_FAT_ENTRY, 1, f);
        fat_data = htonl(fat_data);
        if (fat_data == FAT_AVAILABLE) {
            free++;
        }
    }

    if (free * sb.block_size < size_required) {
        printf("not enough space in image for file\n");
        exit(1);
    }

    int to_write = size_required;
    int reads = 0;
    int pre_block, cur_block;
    char buffer[2*sb.block_size];

    // file = f source = s
    fseek(f, htonl((sb.fat_start * sb.block_size) + (SIZE_FAT_ENTRY * cur_block)), SEEK_SET);
    fread(&cur_block, SIZE_FAT_ENTRY, 1, f);

    fseek(s, 0, SEEK_SET);

    cur_block = next_free_block(&sb.fat_start, sb.fat_blocks);
    dir.start_block = cur_block;

    for (;;) {

        if (to_write < 0) {
            cur_block = FAT_LASTBLOCK;
            fwrite(&cur_block, SIZE_FAT_ENTRY, 1, f);
            break;
        }

        // Read source
        fseek(s, (reads++ * sb.block_size), SEEK_SET);
        fread(&buffer, sb.block_size, 1, s);

        // Write to image
        fseek(f, cur_block * sb.block_size, SEEK_SET);
        fwrite(&buffer, sb.block_size, 1, f);        

        // Update FAT
        to_write = to_write - sb.block_size;

        pre_block = cur_block;
        cur_block = next_free_block(&sb.fat_start, sb.fat_blocks);

        fseek(f, (sb.fat_start * sb.block_size) + (SIZE_FAT_ENTRY * pre_block), SEEK_SET);
        fwrite(&cur_block, SIZE_FAT_ENTRY, 1, f);
    }

    // Update directory 
    dir.status = DIR_ENTRY_NORMALFILE;
    dir.num_blocks = reads + 1;
    dir.file_size = size_required;
    pack_current_datetime(dir.create_time);
    pack_current_datetime(dir.modify_time);
    strcpy(dir.filename, filename);

    fseek(f, sb.dir_start * sb.block_size, SEEK_SET);
    directory_entry_t tmp;
    for (i=0; i<num_enteries; i++) {
        fread(&tmp, sizeof(directory_entry_t), 1, f);
        if (tmp.status == DIR_ENTRY_AVAILABLE) {
            fseek(f, sb.dir_start + (i*sizeof(directory_entry_t)), SEEK_SET);
            fwrite(&dir, sizeof(directory_entry_t), 1, f);
        }
    }
    
    return 0; 
}
