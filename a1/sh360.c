/*
 * Joel Kerfoot
 * CSC 360 A1
 */

// Max # args: 7
// Max len input: 80 chars
// Max len promt: 10 chars
// Max # direct: 10

#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#define MAX_INPUT_LINE 80
#define MAX_NUM_TOKENS 7

	
int main(int argc, char *argv[]) {
	
	char input[MAX_INPUT_LINE];
	char *token[MAX_NUM_TOKENS];
	char *t;
	int i;
	int line_len;
	int num_tokens;
	
	for(;;) {

		fprintf(stdout, "> ");
		fflush(stdout);
		fgets(input, MAX_INPUT_LINE, stdin);
		
		if(input[strlen(input) - 1] == '\n') {
			input[strlen(input) - 1] = '\0';
		}
		
		num_tokens = 0;
		t = strtok(input, "->");
		
		while(t != NULL && num_tokens < MAX_NUM_TOKENS) {
			token[num_tokens] = t;
			num_tokens++;
			t = strtok(NULL, "");
		}// End while
		
		for(i = 0; i < num_tokens; i++) {
			if(strcmp(token[i], "quit") == 0) {
				exit(0);			
			}
			printf("%d: %s\n", i, token[i]);
		}// End for
	}// End For	
}// End main


