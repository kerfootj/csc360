/*
 * Appendix F program.
 * 
 * CSC 360, Summer 2017
 *
 * This shows how we can use the stat() system call to determine 
 * whether or not a file exists and whether or not we are permitted
 * to use the file as an executable.
 *
 * It is far clearer to use this than to work through the error values
 * returned by any of the exec functions.
 */

#include <stdio.h>
#include <sys/stat.h>

/* Should be readable and executable. */
#define FILENAME1 "/bin/ls"

/* Should be readable but not executable. */
#define FILENAME2 "/home/zastre/csc360/a1/appendix_a.c"

/* Should not even exist. */
#define FILENAME3 "/home/zastre/csc360/a1/appendix_z.c"

/* A function prototype, just to be niiiiiice. */
void check_for_file(char *);


/*
 * When checking for whether or not a file is executable, the code
 * below assumes the user-id of the process is not the same as the
 * user-id of the executable (i.e., we must check whether or not
 * the "R" and "X" bits are set for "other" on the file.)
 */

void check_for_file(char *filename) {
    struct stat file_stat;

    if (stat(filename, &file_stat) != 0) {
        printf("%s doesn't exist, or no access to file/directory\n",
            filename);
    } else {
        printf("%s definitely does exist\n",
            filename);
        printf("%s (%s)\n", filename,
            (file_stat.st_mode & S_IROTH ? "read" : "no read")
        );
        printf("%s (%s)\n", filename,
            (file_stat.st_mode & S_IXOTH ? "execute" : "no execute")
        );
    }

    printf("\n");
}


int main() {
    check_for_file(FILENAME1);
    check_for_file(FILENAME2);
    check_for_file(FILENAME3);
}

