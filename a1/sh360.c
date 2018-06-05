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
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>

#define MAX_INPUT_LINE 80
#define MAX_NUM_TOKENS 27
#define MAX_NUM_ARGS 7

void find_commands(int *arr, char **token, int len) {
	int n = 1;	
	for(int i = 1; i < len; i++) {
		if(strcmp(token[i], "->") == 0){
			arr[n++] = i+1;
		} 	
	}
}

void process_args(char *args[], char **token, int start, int end) {
	for(int i = 0; i < end - start; i++) {
		if(i >= MAX_NUM_ARGS)
			return;		
		args[i] = token[start+i];
	}
}

void PP(char **token, int len) {
	
	int cmds[] = {1, -1, -1};
	find_commands(cmds, token, len);
	
	if(cmds[1] == -1) {
		printf("Missing '->' for pipe\n");
		return;
	}
	if(cmds[1] == 2) {
		printf("Missing command before '->'\n");
		return;		
	}
	if(cmds[1] >= len || cmds[2] >= len) {
		printf("Missing command after last '->'\n");
		return;
	}
	if(cmds[2] - cmds[1] == 1) {
		printf("Missing command between '->'  '->'\n");
		return;
	}
	
	int t;

	char *args_head[(cmds[1]-1) - (cmds[0]+1)]; 
	process_args(args_head, token, cmds[0]+1, cmds[1]-1);
	
	if(cmds[2] != -1) {
		char *args_midd[(cmds[2]-1) - (cmds[1]+1)];
		process_args(args_midd, token, (cmds[1]+1), (cmds[2]-1));
		
		t = (cmds[2]-1);
	}else{
		t = (cmds[1]-1);
	}
	
	char *args_tail[len - t];
	process_args(args_head, token, t, len);

	char *test[] = {"-la"}; 

	char *cmd_1[] = {"/bin/ls", *test, 0};
	char *cmd_2[] = {"/usr/bin/sort", NULL, 0};
	char *cmd_3[] = {"/usr/bin/wc", "-l", 0};
	char *envp[] = { 0 };

	int status;
	int n = 0;
	int pid_1, pid_2, pid_3;
	int fd[2];

	pipe(fd);

	printf("parent: setting up piped commands...\n");
	if((pid_1 = fork()) == 0) {
		printf("child (1): re-routing plumbing; STDOUT to pipe.\n");
		dup2(fd[1], 1);
		close(fd[0]);
		execve(cmd_1[0], cmd_1, envp);
		printf("child (1): SHOULDN'T BE HERE.\n");	
	}
	
	if((pid_2 = fork()) == 0) {
		printf("child (2): re-routing plumbing; pipe to STDIN.\n");
		dup2(fd[0], 0);
		close(fd[1]);
		printf("child (2): re-routing plumbing; STDOUT to pipe.\n");
		dup2(fd[1], 1);
		close(fd[0]);
		execve(cmd_2[0], cmd_2, envp);
		printf("child (2): SHOULDN'T BE HERE.\n");
	}
	
	if((pid_3 = fork()) == 0) {
		printf("child (3): re-routing plumbing; pipe to STDIN.\n");
		dup2(fd[0], 0);
		close(fd[1]);
		execve(cmd_3[0], cmd_3, envp);
		printf("child (3): SHOULDN'T BE HERE.\n");
	}

	close(fd[0]);
	close(fd[1]);

	printf("parent: waiting for child (1) to finish...\n");
    waitpid(pid_1, &status, 0);
    printf("parent: child (1) is finished.\n");

	printf("parent: waiting for child (2) to finish...\n");
    waitpid(pid_2, &status, 0);
    printf("parent: child (2) is finished.\n");

    printf("parent: waiting for child (3) to finish...\n");
    waitpid(pid_3, &status, 0); 
    printf("parent: child (3) is finished.\n");

}// End PP

void OR() {
	printf("OR \n");
}// End OR

int tokenize_string(char *s, char *arr[], char *delm, int num) {
	
	char *t;
	int n = 0;

	t = strtok(s, delm);

	while(t != NULL && n < num) {
		arr[n++] = t;
		t = strtok(NULL, delm);
	}
	return n;
}

// https://stackoverflow.com/questions/15515088/how-to-check-if-string-starts-with-certain-string-in-c/15515276
bool starts_with(const char *a, const char *b) {

	if(strncmp(a, b, strlen(b)) == 0)
		return 1;
	return 0;
}

int main(int argc, char *argv[]) {
	
	char input[MAX_INPUT_LINE];
	char *token[MAX_NUM_TOKENS];
	char delim[] = " ";

	int num_tokens;
	
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

		num_tokens = tokenize_string(input, token, delim, MAX_NUM_TOKENS);

		if(strcmp(token[0], "PP") == 0) {
			PP(token, num_tokens);			
		} 
		else if(strcmp(token[0], "OR") == 0) {
			OR(token, num_tokens);
		}
		else {
			printf("Error: %s - Command not recognized\n", token[0]); 		
		}
	}// End For	
}// End main


