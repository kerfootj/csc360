/****************************************************************************
 * File Name		: sh360.c 												*
 * Class			: CSC 360												*
 * Assignment		: 1														*
 * Date				: June 6, 2018 											*
 * Author			: Joel kerfootj 										*
 * Git Repo			: https://github.com/kerfootj/csc360					*
 ****************************************************************************/

/******************************** REFERENCES *******************************/

// [1] https://stackoverflow.com/questions/298510/how-to-get-the-current-directory-in-a-c-program
// [2] https://stackoverflow.com/questions/21260735/how-to-invoke-function-from-external-c-file-in-c/21260908#21260908
// [3] https://stackoverflow.com/questions/14179559/changing-working-directory-in-c
// [4] https://stackoverflow.com/questions/298510/how-to-get-the-current-directory-in-a-c-program
// [5] http://homepages.inf.ed.ac.uk/dts/pm/Papers/nasa-c-style.pdf
// [6] https://linux.die.net/man/2/chdir 
// [7] https://linux.die.net/man/2/mkdir
// [6] linux.csc.uvic.ca/home/zastre/csc360/a1

/******************************** LIBRARIES ********************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>

#include "sh360plus.c"

/******************************** CONSTANTS ********************************/

#define MAX_NUM_ARGS 7
#define MAX_LINE_LENGTH 80
#define MAX_PROMPT_LENGTH 10
#define MAX_NUM_DIRS_IN_PATH 10

/*************************** GLOBAL VARIABLES *****************************/

char g_prompt[MAX_PROMPT_LENGTH];
char g_dirs[MAX_NUM_DIRS_IN_PATH][MAX_LINE_LENGTH];

int g_numDirs;

/******************************* PROTOTYPES *******************************/

int read_config(); 

int find_path(char *binary, char *fullpath);
int tokenize_string(char **token, char *cmd);

int run_cmd(char *cmd);
int exec_cmd(char *binary, char ** args, int num_tokens);

int piping(char *cmd);
int exec_pipe_2(char **token, int delim, int num_tokens);
int exec_pipe_3(char ** token, int delim1, int delim2, int num_tokens);

int output_redirect(char *cmd);

/*************************** IMPLEMENTATION *****************************/

// Format the end of a line
void end_line(char * str) {
	if (str[strlen(str) - 1] == '\n') {
        str[strlen(str) - 1] = '\0';
    }
}

// Read .sh360rc and set up prompt and directories 
int read_config() {
	
	FILE *fp;
	if(!(fp = fopen(".sh360rc", "r"))) {
		fprintf(stderr, ".sh360rc not found");
		return 0;
	}

	fgets(g_prompt, MAX_PROMPT_LENGTH, fp);
	end_line(g_prompt);
	
	g_numDirs = 0;

	while(fgets(g_dirs[g_numDirs], sizeof g_dirs[g_numDirs], fp) && g_numDirs < MAX_NUM_DIRS_IN_PATH) {
		end_line(g_dirs[g_numDirs]);
		g_numDirs++;
	}
	
	return 1;
}

// Find full path for command if it exists 
int find_path(char *bin, char *fullpath) {
	
	// Check that bin is already the full path
	FILE *fp = fopen(bin, "r");
	if(fp) {
		memcpy(fullpath, bin, strlen(bin)+1);
		fclose(fp);
		return 1;
	}
	
	struct stat file_stat;

	// Determine if the path exist and is readable and executable
	char test[MAX_LINE_LENGTH];
	int i;

	for(i = 0; i < g_numDirs; i++) {
	
		test[0] = '\0';
		strcat(test, g_dirs[i]);
		strcat(test, "/");
		strcat(test, bin);
		strcat(test, "\0");

		if(stat(test, &file_stat) == 0) {
			if(file_stat.st_mode & S_IXOTH) {
				memcpy(fullpath, test, strlen(test)+1);
				return 1;	
			}
		}
	}

	memcpy(fullpath, "\0", 1);
	return 0;
}

// Toekenize string by spaces 
int tokenize_string(char **token, char *cmd) {
	char *t;
	int num_tokens = 0;

	// Tokenize the string by sperating at spaces
	t = strtok(cmd, " ");
	while(t != NULL && num_tokens < 30) {
		token[num_tokens++] = t;
        t = strtok(NULL, " ");
	}

	token[num_tokens] = NULL;

	return num_tokens;
}

// Process a single command for execution
int run_cmd(char *cmd) {
	
	char *token[MAX_NUM_ARGS+2];
	int num_tokens = tokenize_string(token, cmd);
	
	char path[MAX_LINE_LENGTH];
	path[0] = '\0';

	int found = find_path(token[0], path);

	if(found) {
		exec_cmd(path, token, num_tokens);
		return 1;
	} else {
		fprintf(stderr, "Invalid Command: '%s'\n", token[1]);
		return 0;
	}
	return 1;
}

// Execute a single command passed directly from the propmt
int exec_cmd(char *binary, char ** args, int num_tokens) {
	char *envp[] = { 0 };
	int status;
	int pid;

	if((pid = fork()) == 0) {
		args[0] = binary;
		args[num_tokens] = 0;

		if(execve(args[0], args, envp) == -1) {
			fprintf(stderr, "Error: execve failed.\n");
			exit(1);
		}
	}
	waitpid(pid, &status, 0);

	return 1;
}

// Initial setup for implimenting piping
int piping(char *cmd) {
	
	char *token[27];
	int num_tokens = tokenize_string(token, cmd);

	int delim_1 = -1;
	int delim_2 = -1;
	int i = 0;

	// Determine number of pipes
	while(token[i] != NULL) {
		if(strcmp(token[i], "->") == 0){
			if(delim_1 == -1)
				delim_1 = i;
			else
				delim_2 = i;
		}
		i++;
	}

	int multi = (delim_2 != -1) ? 2 : 1;

	if(delim_1 == -1) {
		fprintf(stderr, "PP missing '->'. Usage: PP <command> -> <command>\n");
		return 0;
	}
	if(delim_1 == 1) {
		fprintf(stderr, "PP missing <command> before '->'. Usage: PP <command> -> <command>\n");
		return 0;
	}
	if(delim_1 >= num_tokens || delim_2 >= num_tokens) {
		fprintf(stderr, "PP missing <command> after last '->'. Usage: PP <command> -> <command> (-> <command>)\n");
		return 0;
	}
	if(delim_2 - delim_1 == 1 && multi == 2) {
		fprintf(stderr, "PP missing <command> between '->'. Usage: PP <command> -> <command> -> <command>\n");
		return 0;
	}
	
	if(multi == 1) 
		return exec_pipe_2(token, delim_1, num_tokens);
	else if (multi == 2) 
		return exec_pipe_3(token, delim_1, delim_2, num_tokens);
	return 0;
}

// Pipe the first command into the second
int exec_pipe_2(char **token, int delim, int num_tokens) {
	
	char *cmd_head[delim];
	char *cmd_tail[num_tokens-delim];

	// Seperate the args for each command
	int i;
	for(i = 1; i < delim; i++)
		cmd_head[i-1] = token[i];
	cmd_head[delim-1] = 0;

	for(i = delim+1; i < num_tokens; i++)
			cmd_tail[i-delim-1] = token[i];
	cmd_tail[num_tokens-delim-1] = 0;

	// Get paths for binaries
	char path_head[MAX_LINE_LENGTH];
	char path_tail[MAX_LINE_LENGTH];

	int f1 = find_path(cmd_head[0], path_head);
	int f2 = find_path(cmd_tail[0], path_tail);

	if(!f1) {
		fprintf(stderr, "Invalid Command '%s' before '->'\n", cmd_head[0]);
		return 0;
	}
	if(!f2) {
		fprintf(stderr, "Invalid Command '%s' after '->'\n", cmd_tail[0]);
		return 0;
	}

	int pid_head, pid_tail;
	int status;
	int fd[2];

	char *envp[] = { 0 };

	// Create Pipe
	pipe(fd);

	if((pid_head = fork()) == 0) {
		dup2(fd[1], 1);
        close(fd[0]);
        execve(path_head, cmd_head, envp);
	}
	if ((pid_tail = fork()) == 0) {
		dup2(fd[0], 0);
        close(fd[1]);
        execve(path_tail, cmd_tail, envp);
	}

	// Close all file descriptors
	close(fd[0]);
    close(fd[1]);

    // Wait for child process to complete
    waitpid(pid_head, &status, 0);
    waitpid(pid_tail, &status, 0); 

    return 1;
}

// Pipe through the three commands
int exec_pipe_3(char ** token, int delim1, int delim2, int num_tokens) {

	char *cmd_head[delim1];
	char *cmd_midd[delim2-delim1];
	char *cmd_tail[num_tokens-delim2];

	// Seperate the args for each command
	int i;
	for(i = 1; i < delim1; i++)
		cmd_head[i-1] = token[i];
	cmd_head[delim1-1] = 0;

	for(i = delim1+1; i < delim2; i++)
		cmd_midd[i-delim1-1] = token[i];
	cmd_midd[delim2-delim1-1] = 0;

	for(i = delim2+1; i < num_tokens; i++)
		cmd_tail[i-delim2-1] = token[i];
	cmd_tail[num_tokens-delim2-1] = 0; 

	// Get paths for binaries
	char path_head[MAX_LINE_LENGTH];
	char path_midd[MAX_LINE_LENGTH];
	char path_tail[MAX_LINE_LENGTH];

	int f1 = find_path(cmd_head[0], path_head);
	int f2 = find_path(cmd_midd[0], path_midd);
	int f3 = find_path(cmd_tail[0], path_tail);

	if(!f1) {
		fprintf(stderr, "Invalid Command '%s' before '->'\n", cmd_head[0]);
		return 0;
	}
	if(!f2) {
		fprintf(stderr, "Invalid Command '%s' between '->'\n", cmd_midd[0]);
		return 0;
	}
	if(!f3) {
		fprintf(stderr, "Invalid Command '%s' after '->'\n", cmd_tail[0]);
		return 0;
	}

	int pid_head, pid_midd, pid_tail;
	int status;
	int fd[4];

	char *envp[] = { 0 };

	// Create pipes
	pipe(fd);
	pipe(fd+2);

	if((pid_head = fork()) == 0) {
		
		// Pipe output
		dup2(fd[1], 1);
 		close(fd[0]);
 		execve(path_head, cmd_head, envp);
	}

	if((pid_midd = fork()) == 0) {
		
		// Pipe input
		dup2(fd[0], 0);
		close(fd[1]);

		// Pipe output
		dup2(fd[3], 1);
		close(fd[2]);
		execve(path_midd, cmd_midd, envp);
	}

	if((pid_tail = fork()) == 0) {

		// Pipe input
		dup2(fd[2], 0);
		close(fd[1]);
		close(fd[3]);
		execve(path_tail, cmd_tail, envp);
	}

	// Close all file descriptors
	for(i = 0; i < 4; i++)
		close(fd[i]);
	
	// Wait for child process to complete
    waitpid(pid_head, &status, 0);
    waitpid(pid_midd, &status, 0);
    waitpid(pid_tail, &status, 0); 

    return 1;
}

// Redirect command output to a file
int output_redirect(char *cmd) {
	
	char *token[MAX_NUM_ARGS+2];
	int num_tokens = tokenize_string(token, cmd);
	int i;

	char path[MAX_LINE_LENGTH];
	path[0] = '\0';

	if(strcmp(token[num_tokens - 2], "->") != 0) {
		fprintf(stderr, "Invalid Syntax. OR missing '->'. Usage: OR <command> -> <destination>\n");
		return 0;
	}
	if(num_tokens < 4) {
		fprintf(stderr, "Invalid syntax. Usage: OR <command> -> <destination>\n");
		return 0;
	}

	char *args[num_tokens - 2];
	for(i = 1; i < num_tokens - 2; i++) {
		args[i-1] = token[i];
	}

	args[num_tokens-3] = NULL;
	char *out = token[num_tokens-1];

	int found = find_path(args[0], path);

	if(found){

		char *envp[] = { 0 };
		int status;
		int pid;


		if((pid = fork()) == 0) {

			int fd = open(out, O_CREAT|O_RDWR, S_IRUSR|S_IWUSR);
			if(fd == -1) {
				fprintf(stderr, "Cannot open %s for writing\n", out);
            	exit(1);
			}

			dup2(fd, 1);
        	dup2(fd, 2); 
        	execve(path, args, envp);
		}

		waitpid(pid, &status, 0);
	} else {
		fprintf(stderr, "Invalid Command: '%s'\n", token[1]);
		return 0;
	}

	return 1;
}

int main(int argc, char *argv[]) {

	read_config();

	for(;;) {
		
		fprintf(stdout, "%s ", g_prompt);
		fflush(stdout);
	
		char input[MAX_LINE_LENGTH];
		
		fgets(input, MAX_LINE_LENGTH, stdin);
		end_line(input);

		if(strcmp(input, "exit") == 0)
			exit(0);
		else if(strncmp(input, "PP", 2) == 0)
			piping(input);
		else if(strncmp(input, "OR", 2) == 0)
			output_redirect(input);
		else if(strncmp(input, "CD", 2) == 0){
			char *token[MAX_NUM_ARGS];
			int num_tokens = tokenize_string(token, input);
			change_directory(token, num_tokens);
		} else if(strncmp(input, "MKDIR", 2) == 0){
			char *token[MAX_NUM_ARGS];
			int num_tokens = tokenize_string(token, input);
			make_directory(token, num_tokens);
		} else
			run_cmd(input);
	}
	return 0;
}
