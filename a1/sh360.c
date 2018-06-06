/*
 * Joel Kerfoot
 * CSC 360 A1
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include <stdbool.h>

#define MAX_INPUT_LINE 80
#define MAX_NUM_TOKENS 27
#define MAX_NUM_ARGS 7
#define MAX_NUM_DIRECTORIES 10
#define MAX_LEN_PATH 80
#define MAX_LEN_PROMT 10

void command_positions(int *arr, char **token, int len) {
	int n = 1;	
	for(int i = 1; i < len; i++) {
		if(strcmp(token[i], "->") == 0){
			arr[n++] = i+1;
		} 	
	}
}

void process_args(char *args[], char **token, int start, int end) {
	for(int i = 0; i < MAX_NUM_ARGS; i++) {
		if(i < end - start)
			args[i] = token[start+i];
		else
			args[i] = NULL;
	}
}

bool validate_command(char **cmd) {

	if(*cmd[strlen(*cmd) - 1] == '\n') {
		*cmd[strlen(*cmd) - 1] = '\0';
	}

	struct stat file_stat;
	
	FILE *rc;
	rc = fopen(".sh360rc", "r");

	char filename[MAX_LEN_PATH];
	int n = 0;

	while(fgets(filename, sizeof filename, rc) != NULL) {
		if(n > 0 && n <= MAX_NUM_DIRECTORIES) {
			strtok(filename, "\n"); // Remove new line character
			if(filename[strlen(filename)] != '/') {
				strcat(filename, "/");
			}
			strcat(filename, *cmd);
			
			if(stat(filename, &file_stat) == 0){
				if(file_stat.st_mode & S_IXOTH) {
					*cmd = filename;
					printf("cmd: %s\n", *cmd);
					return true;				
				}		
			}
			n++;
		} else if(n > MAX_NUM_DIRECTORIES) {
			break;
		} else {
			n++;		
		}
	}
	printf("Command: %s not recognized\nCheck it's path is in '.sh360rc'\n", *cmd);
	return false;
}

void PP(char **token, int len) {
	
	int cmds[] = {1, -1, -1};
	command_positions(cmds, token, len);
	
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
	
	bool multi = (cmds[2] != -1) ? true : false;
	int num = (multi) ? 2 : 1;
	int t;
			
	// Seperate arguments from commands
	char *args_head[MAX_NUM_ARGS]; 
	process_args(args_head, token, cmds[0]+1, cmds[1]-1);
	
	char *args_midd[MAX_NUM_ARGS];
	if(multi) {
		process_args(args_midd, token, (cmds[1]+1), (cmds[2]-1));
		t = (cmds[2]+1);
	}else{
		t = (cmds[1]+1);
	}
	
	char *args_tail[MAX_NUM_ARGS];
	process_args(args_tail, token, t, len);
	
	// Commands to pass to execve
	//char *cmd_head[] = {token[cmds[0]], *args_head, 0};
	//char *cmd_tail[] = {token[cmds[num]] , *args_tail, 0};  
	
	char *test = "/bin/ls";
	char *path_head = token[cmds[0]];
	validate_command(&path_head);
	char *test2 = path_head;

	//printf("%d\n", strcmp(path_head, test));

	char *cmd_head[] = {test2, *args_head, 0};
	char *cmd_tail[] = {"/usr/bin/wc", *args_tail, 0}; 
	char *cmd_midd[3];
	
	if(multi) {
		cmd_midd[0] = token[cmds[1]];
		cmd_midd[1] = *args_midd;
		cmd_midd[2] = 0;	
	}
	
	char *envp[] = { 0 };
	
	int status;
	int pid_1, pid_2, pid_3;
	int fd[2*num];

	pipe(fd);
	if(multi) { pipe(fd+2); }

	printf("parent: setting up piped commands...\n");
	if((pid_1 = fork()) == 0) {
		printf("child (1): re-routing plumbing; STDOUT to pipe.\n");
		dup2(fd[1], 1);
		close(fd[0]);
		execve(cmd_head[0], cmd_head, envp);
		printf("child (1): SHOULDN'T BE HERE.\n");	
	}
	
	if(multi) {
		if((pid_2 = fork()) == 0) {
			printf("child (2): re-routing plumbing; pipe to STDIN.\n");
			dup2(fd[0], 0);
			close(fd[1]);
			printf("child (2): re-routing plumbing; STDOUT to pipe.\n");
			dup2(fd[3], 1);
			close(fd[2]);
			execve(cmd_midd[0], cmd_midd, envp);
			printf("child (2): SHOULDN'T BE HERE.\n");
		}
	}
	
	if((pid_3 = fork()) == 0) {
		printf("child (3): re-routing plumbing; pipe to STDIN.\n");
		if(multi) {
			dup2(fd[2], 0);
			close(fd[1]);
			close(fd[3]);	
		} else {		
			dup2(fd[0], 0);
			close(fd[1]);
		}
		execve(cmd_tail[0], cmd_tail, envp);
		printf("child (3): SHOULDN'T BE HERE.\n");
	}

	close(fd[0]);
	close(fd[1]);
	if(multi) {
		close(fd[2]);
		close(fd[3]);
	}

	printf("parent: waiting for child (1) to finish...\n");
    waitpid(pid_1, &status, 0);
    printf("parent: child (1) is finished.\n");
	
	if(multi) {
		printf("parent: waiting for child (2) to finish...\n");
		waitpid(pid_2, &status, 0);
		printf("parent: child (2) is finished.\n");
	}
	
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
	char prompt[MAX_LEN_PROMT];
	char delim[] = " ";

	int num_tokens;

	FILE *rc;
	rc = fopen(".sh360rc", "r");
	fscanf(rc, "%s", prompt);
	fclose(rc);
	
	for(;;) {

		fprintf(stdout, "%s", prompt);
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


