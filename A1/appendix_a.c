/*
 * Appendix A program.
 * CSC 360, Summer 2017
 *
 * This shows how a simple loop obtaining input from the user can
 * be written. Notice the use of "fflush".
 *
 * If "impeach" is entered, then the shell terminates.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_INPUT_LINE 100


int main(int argc, char *argv[]) {
    char input[MAX_INPUT_LINE];
    int  line_len;

    for(;;) {
        fprintf(stdout, "> ");
        fflush(stdout);
        fgets(input, MAX_INPUT_LINE, stdin);
        if (input[strlen(input) - 1] == '\n') {
            input[strlen(input) - 1] = '\0';
        }

        line_len = strlen(input); 
        fprintf(stdout, "echo: line was %d chars long\n", line_len);

        if (strcmp(input, "impeach") == 0) {
            exit(0);
        } 
    }
}
