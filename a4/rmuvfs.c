#include <arpa/inet.h>
#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include "disk.h"


int erase_data(FILE *f, superblock_entry_t sb, directory_entry_t dir, int offset) {
    
    int reads = 0;
    int cur_block; int nxt_block;
    int to_erase = dir.file_size;
    
    char buffer[sb.block_size];
    memset(buffer, 0, sizeof(buffer));

    cur_block = dir.start_block;

    fseek(f, (sb.fat_start * sb.block_size) + (SIZE_FAT_ENTRY * cur_block), SEEK_SET);
    fread(&nxt_block, sizeof(unsigned int), 1, f);
    nxt_block = htonl(nxt_block);

    for (;;) {    
        
        // Erase data
        fseek(f, (cur_block * sb.block_size), SEEK_SET);
        fwrite(&buffer, sb.block_size, 1, f);

        // What's left to erase
        to_erase = to_erase - sb.block_size;

        // Remove FAT entry
        fseek(f, ((sb.fat_start * sb.block_size) + (cur_block * SIZE_FAT_ENTRY)), SEEK_SET);
        fwrite(&buffer, sizeof(unsigned int), 1, f);

        if (to_erase < 0) {
            break;
        }

        cur_block = nxt_block;
        fseek(f, (sb.fat_start * sb.block_size) + (SIZE_FAT_ENTRY * cur_block), SEEK_SET);
        fread(&nxt_block, sizeof(unsigned int), 1, f);
        nxt_block = htonl(nxt_block);
    }

    // Remove directory entry
    fseek(f, (sb.dir_start * sb.block_size) + offset, SEEK_SET);
    fwrite(&buffer, SIZE_DIR_ENTRY, 1, f);

    return reads;
}


int main(int argc, char *argv[]) {
     superblock_entry_t sb;
    int  i;
    char *imagename = NULL;
    char *filename  = NULL;
    FILE *f;

    for (i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--image") == 0 && i+1 < argc) {
            imagename = argv[i+1];
            i++;
        } else if (strcmp(argv[i], "--file") == 0 && i+1 < argc) {
            filename = argv[i+1];
            i++;
        }
    }

    if (imagename == NULL || filename == NULL) {
        fprintf(stderr, "usage: rmuvfs --image <imagename> " \
            "--file <filename in image>\n");
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

    dir.start_block = htonl(dir.start_block);
    dir.num_blocks = htonl(dir.num_blocks);
    dir.file_size = htonl(dir.file_size);

    // Check if filename already exists
    int found = 0;
    int offset = 0;

    // Find the file
    for (i=0; i<num_enteries; i++) {
        fread(&dir, sizeof(directory_entry_t), 1, f);
        if (dir.status != DIR_ENTRY_AVAILABLE) {
            if (strncmp(dir.filename, filename, strlen(filename) +1) == 0) {
                found = 1;
                offset = i * SIZE_DIR_ENTRY;
                break;
            }
        }
    }

    if (!found) {
        printf("file: %s not found\n", filename);
        exit(1);
    }

    dir.start_block = htonl(dir.start_block);
    dir.num_blocks = htonl(dir.num_blocks);
    dir.file_size = htonl(dir.file_size);

    erase_data(f, sb, dir, offset);
    
    return 0; 
}
