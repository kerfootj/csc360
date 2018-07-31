#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <string.h>
#include "disk.h"


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
        fprintf(stderr, "usage: catuvfs --image <imagename> " \
            "--file <filename in image>");
        exit(1);
    }


    // Open binary
    f = fopen(imagename, "r");
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

    // Find the file
    for (i=0; i<num_enteries; i++) {
        fread(&dir, sizeof(directory_entry_t), 1, f);
        if (dir.status != DIR_ENTRY_AVAILABLE) {
            if (strncmp(dir.filename, filename, strlen(filename) +1) == 0) {
                found = 1;
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
    
    int cur_block = dir.start_block;
    char buffer[2*sb.block_size];

    fseek(f, htonl((sb.fat_start * sb.block_size) + (SIZE_FAT_ENTRY * cur_block)), SEEK_SET);
    fread(&cur_block, SIZE_FAT_ENTRY, 1, f);

    for(;;) {

        // Read data
        fseek(f, cur_block * sb.block_size, SEEK_SET);
        fread(buffer, sb.block_size, 1, f);

        // Print data
        printf("%s", buffer);

        // Find next block
        fseek(f, (sb.fat_start * sb.block_size) + (SIZE_FAT_ENTRY * cur_block), SEEK_SET);
        fread(&cur_block, sizeof(unsigned int), 2, f);

        cur_block = htonl(cur_block);

        if (cur_block == FAT_LASTBLOCK) {
            break;
        }
    }

    return 0; 
}
