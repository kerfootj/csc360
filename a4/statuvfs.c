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
    FILE  *f;
    int   fat_data;

    for (i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--image") == 0 && i+1 < argc) {
            imagename = argv[i+1];
            i++;
        }
    }

    if (imagename == NULL)
    {
        fprintf(stderr, "usage: statuvfs --image <imagename>\n");
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

    char *heading[] = {"Bsz", "Bcnt", "FATst", "FATcnt", "DIRst", "DIRcnt", "Free", "Resv", "Alloc"};

    printf("%s (%s)\n\n", sb.magic, imagename);
    printf("-------------------------------------------------\n");
    printf("%5s %6s %6s %6s %6s %6s\n",
    	heading[0],
    	heading[1],
    	heading[2],
    	heading[3],
    	heading[4],
    	heading[5]);

    printf("%5d %6d %6d %6d %6d %6d\n\n",
    	sb.block_size,
        sb.num_blocks,
        sb.fat_start,
        sb.fat_blocks,
        sb.dir_start,
        sb.dir_blocks);

    printf("-------------------------------------------------\n");

	int free = 0;
	int resv = 0;
	int aloc = 0;

    fseek(f, sb.fat_start * sb.block_size, SEEK_SET);
 	for (i=0; i<sb.num_blocks; i++) {
 		fread(&fat_data, SIZE_FAT_ENTRY, 1, f);
 		fat_data = htonl(fat_data);
 		if (fat_data == FAT_AVAILABLE)
 			free++;
 		else if (fat_data == FAT_RESERVED)
 			resv++;
 		else
 			aloc++;
 	}

    printf("%5s %6s %6s\n", heading[6], heading[7], heading[8]);
    printf("%5d %6d %6d\n\n", free, resv, aloc);

    return 0; 
}
