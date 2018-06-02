/*
 * Joel Kerfoot
 * CSC 360 A1
 */

// Max # args: 7
// Max len input: 80 chars
// Max len promt: 10 chars
// Max # direct: 10

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>

#define MAX_INPUT_LINE 80
#define MAX_NUM_TOKENS 7

void PP(char input[]) {
	char *cmd_head[] = {"/bin/ls", "-1", 0};
	char *cmd_tail[] = {"/usr/bin/wc", "-l", 0};
	char *envp[] = { 0 };
	int status;
	int pid_head, pid_tail;
	int fd[2];

	pipe(fd);

	printf("parent: setting up piped commands...\n");
	if((pid_head = fork()) == 0) {
		printf("child (head): re-routing plumbing; STDOUT to pipe.\n");
		dup2(fd[1], 1);
		close(fd[0]);
		execve(cmd_head[0], cmd_head, envp);
		printf("child (head): SHOULDN'T BE HERE.\n");	
	}
	
	if((pid_tail = fork()) == 0) {
		printf("child (tail): re-routing plumbing; pipe to STDIN.\n");
		dup2(fd[0], 0);
		close(fd[1]);
		execve(cmd_tail[0], cmd_tail, envp);
		printf("child (tail): SHOULDN'T BE HERE.\n");
	}

	close(fd[0]);
	close(fd[1]);

	printf("parent: waiting for child (head) to finish...\n");
    waitpid(pid_head, &status, 0);
    printf("parent: child (head) is finished.\n");

    printf("parent: waiting for child (tail) to finish...\n");
    waitpid(pid_tail, &status, 0); 
    printf("parent: child (tail) is finished.\n");

}// End PP

void OR() {
	printf("OR \n");
}// End OR

// https://stackoverflow.com/questions/15515088/how-to-check-if-string-starts-with-certain-string-in-c/15515276
bool starts_with(const char *a, const char *b) {

	if(strncmp(a, b, strlen(b)) == 0)
		return 1;
	return 0;
}
	
int main(int argc, char *argv[]) {
	
	char input[MAX_INPUT_LINE];
	char *token[MAX_NUM_TOKENS];
	char *t;

	int i;
	int line_len;
	int num_tokens;
	int cmd;
	
	for(;;) {

		fprintf(stdout, "> ");
		fflush(stdout);
		fgets(input, MAX_INPUT_LINE, stdin);
		
		if(input[strlen(input) - 1] == '\n') {
			input[strlen(input) - 1] = '\0';
		}
		
		// Exit
		if(strcmp(input, "exit") == 0) {
			exit(0);		
		}
		
		if(starts_with(input, "PP ") && strlen(input) > 3) {
			cmd = 3;
			memmove(input, input+3, strlen(input)-2);	
		}else if(starts_with(input, "OR ") && strlen(input) > 3) {
			cmd = 2;
			memmove(input, input+3, strlen(input)-2);	
		}else {
			printf("Error: %s - Command not recognized.\n", input);				
		}
	
		if(cmd == 2 || cmd == 3) {
			
			char delm[] = ">";

			num_tokens = 0;
		    t = strtok(input, delm);
 
			// Tokenize string
			while(t != NULL && num_tokens < MAX_NUM_TOKENS) {
				token[num_tokens] = t;
				num_tokens++;
				t = strtok(NULL, delm);
			}// End while
			
			for (i = 0; i < num_tokens; i++) {
        		printf("%d: %s\n", i, token[i]);
    		}// End for
		}// End if
	}// End For	
}// End main


