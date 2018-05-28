/*
 * Appendix E program.
 * CSC 360, Summer 2017
 *
 * This shows how input from the user can be broken down into individual
 * words / tokens.
 *
 * To understand this code, you must be *completely* convinced that a
 * "char *" variables does *not* store a string. Rather, it stores 
 * an address to memory location storing a character. If the value stored
 * at that memory location changes, but the "char *" value doesn't, then
 * it is the new contents at the memory location which appear, not the old.
 *
 * Important: strtok modifies the char array which is being tokenized.
 * If you need to keep the original string, then make a copy of it before
 * running strtok.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_INPUT_LINE 60
#define MAX_NUM_TOKENS 10


int main(int argc, char *argv[]) {
    char input[MAX_INPUT_LINE];
    char *token[MAX_NUM_TOKENS];
    char *t;
    int  i;
    int  line_len;
    int  num_tokens;

    fprintf(stdout, "> ");
    fflush(stdout);
    fgets(input, MAX_INPUT_LINE, stdin);
    if (input[strlen(input) - 1] == '\n') {
        input[strlen(input) - 1] = '\0';
    }

    num_tokens = 0;
    t = strtok(input, " ");
    while (t != NULL && num_tokens < MAX_NUM_TOKENS) {
        token[num_tokens] = t;
        num_tokens++;
        t = strtok(NULL, " ");
    }

    /* Note that an address stored in some token[i] is actually an address
     * to a char location within the input array.
     */
    for (i = 0; i < num_tokens; i++) {
        printf("%d: %s\n", i, token[i]);
    } 

    /* And to prove the point about my comment just above... */
    memset(input, 0, MAX_INPUT_LINE);
    strncpy(input, "I have been treated *sooooo* unfairly by the press",
        MAX_INPUT_LINE);
    printf("\nWhat follows will only make sense if you trace it through.\n");
    for (i = 0; i < num_tokens; i++) {
        printf("%d: %s\n", i, token[i]);
    } 
}
