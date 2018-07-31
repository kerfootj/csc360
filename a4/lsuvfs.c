#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <string.h>
#include "disk.h"


char *month_to_string(short m) {
    switch(m) {
    case 1: return "Jan";
    case 2: return "Feb";
    case 3: return "Mar";
    case 4: return "Apr";
    case 5: return "May";
    case 6: return "Jun";
    case 7: return "Jul";
    case 8: return "Aug";
    case 9: return "Sep";
    case 10: return "Oct";
    case 11: return "Nov";
    case 12: return "Dec";
    default: return "?!?";
    }
}


void unpack_datetime(unsigned char *time, short *year, short *month, 
    short *day, short *hour, short *minute, short *second)
{
    assert(time != NULL);

    memcpy(year, time, 2);
    *year = htons(*year);

    *month = (unsigned short)(time[2]);
    *day = (unsigned short)(time[3]);
    *hour = (unsigned short)(time[4]);
    *minute = (unsigned short)(time[5]);
    *second = (unsigned short)(time[6]);
}


int main(int argc, char *argv[]) {
    superblock_entry_t sb;
    int  i;
    char *imagename = NULL;
    FILE *f;

    for (i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--image") == 0 && i+1 < argc) {
            imagename = argv[i+1];
            i++;
        }
    }

    if (imagename == NULL)
    {
        fprintf(stderr, "usage: lsuvfs --image <imagename>\n");
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

    for (i=0; i<num_enteries; i++) {
        fread(&dir, sizeof(directory_entry_t), 1, f);

        dir.start_block = htonl(dir.start_block);
        dir.num_blocks = htonl(dir.num_blocks);
        dir.file_size = htonl(dir.file_size);

        if (dir.status != DIR_ENTRY_AVAILABLE) {
            short year, month, day, hour, min, sec;
            unpack_datetime(dir.create_time, &year, &month, &day, &hour, &min, &sec);

            printf("%8d %4d-%s-%d %d:%d:%d %s\n", 
                dir.file_size,
                year,
                month_to_string(month),
                day,
                hour,
                min,
                sec,
                dir.filename);
        }
    }

    return 0; 
}
