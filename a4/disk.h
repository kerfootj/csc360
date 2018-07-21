#ifndef _DISK_H_
#define _DISK_H_ 

#define FILE_SYSTEM_ID_LEN 8
#define FILE_SYSTEM_ID "uvicfs18"

#define MIN_BLOCK_SIZE 64
#define SIZE_FAT_ENTRY 4
#define MAX_FAT 5120
#define FAT_START_BLOCK 1
#define SIZE_DIR_ENTRY 64
#define MAX_DIR_ENTRIES 64

typedef struct superblock_entry superblock_entry_t;
struct superblock_entry {
             char  magic[FILE_SYSTEM_ID_LEN];
    unsigned short block_size;
    unsigned int   num_blocks;
    unsigned int   fat_start;
    unsigned int   fat_blocks;
    unsigned int   dir_start;
    unsigned int   dir_blocks;
} __attribute__ ((packed));


#define FAT_AVAILABLE 0x00000000
#define FAT_RESERVED  0x00000001
#define FAT_LASTBLOCK 0xffffffff

#define DIR_TIME_WIDTH 7
#define DIR_FILENAME_MAX 31
#define DIR_PADDING_SIZE 8
#define DIR_ENTRY_AVAILABLE  0x0
#define DIR_ENTRY_NORMALFILE 0x1
#define DIR_ENTRY_DIRECTORY  0x2

typedef struct directory_entry directory_entry_t;
struct directory_entry {
    unsigned char status;
    unsigned int  start_block;
    unsigned int  num_blocks;
    unsigned int  file_size;
    unsigned char create_time[DIR_TIME_WIDTH];
    unsigned char modify_time[DIR_TIME_WIDTH];
             char filename[DIR_FILENAME_MAX];
    unsigned char _padding[6];
} __attribute__ ((packed));

#endif
