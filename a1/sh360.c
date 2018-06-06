/*
 * Joel Kerfoot
 * CSC 360 A1
 */

/******************************* REFERENCES ******************************/
//
// [1] https://stackoverflow.com/questions/298510/how-to-get-the-current-directory-in-a-c-program
//

/******************************* LIBRARIES *******************************/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>

/******************************* CONSTANTS *******************************/

#define MAX_NUM_ARGS 7
#define MAX_LINE_LENGTH 80
#define MAX_PROMPT_LENGTH 10
#define MAX_NUM_DIRS_IN_PATH 10


/************************** GLOBAL VARIABLES ****************************/

char g_prompt[MAX_PROMPT_LENGTH];
char g_dirs[MAX_NUM_DIRS_IN_PATH][MAX_LINE_LENGTH];

int g_numDirs;

/****************************** PROTOTYPES ******************************/

int read_config();
int read_input(char *input);

int tokenize_string(char **token, char *cmd);
int find_path(char *binary, char *fullpath);

int exec_cmd(char *binary, char ** args, int num_tokens);

/*************************** IMPLEMENTATION *****************************/

void end_line(char * str) {
	if (str[strlen(str) - 1] == '\n') {
        str[strlen(str) - 1] = '\0';
    }
}

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

int find_path(char *bin, char *fullpath) {
	
	// Check that bin is already full path

	FILE *fp = fopen(bin, "r");
	if(fp) {
		memcpy(fullpath, bin, strlen(bin)+1);
		fclose(fp);
		return 1;
	}
	
	struct stat file_stat;

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

int tokenize_string(char **token, char *cmd) {
	char *t;
	int num_tokens = 0;

	t = strtok(cmd, " ");
	while(t != NULL && num_tokens < 30) {
		token[num_tokens++] = t;
        t = strtok(NULL, " ");
	}

	token[num_tokens] = NULL;

	int i;
    for(i = 0; i < num_tokens; i++) {
		char *t = token[i];
		if(strncmp(t, "~", 1) == 0) {
			
			char temp[strlen(t)+1];
			memcpy(temp, t, strlen(t));
			temp[strlen(t)] = '\0';

			t[0] = '\0';
			strcat(t, getenv("HOME"));
			strcat(t, &temp[1]);
    		strcat(t, "\0");

		} else if(strncmp(t, "./", 2) == 0){
			
			char temp[strlen(t) + 1];
    		memcpy(temp, t, strlen(t));
    		temp[strlen(t)] = '\0';
			
			// ref[1]
			char cwd[1024];
    		getcwd(cwd, sizeof(cwd));

    		t[0] = '\0';
    		strcat(t, cwd);
    		strcat(t, &temp[1]);
    		strcat(t, "\0");
		}
	}
	return num_tokens;
}

int run_cmd(char *cmd){
	
	char *token[MAX_NUM_ARGS];
	int num_tokens = tokenize_string(token, cmd);
	
	char to_run[MAX_LINE_LENGTH];
	to_run[0] = '\0';

	int found = find_path(token[0], to_run);

	if(found) {
		exec_cmd(to_run, token, num_tokens);
		return 1;
	} else {
		fprintf(stderr, "Invalid command.\n");
		return 0;
	}

	return 1;
}

int exec_cmd(char *binary, char ** args, int num_tokens) {
	char *envp[] = { 0 };
	int pid;

	if((pid = fork()) == 0) {
		args[0] = binary;
		args[num_tokens] = 0;

		if(execve(args[0], args, envp) == -1) {
			fprintf(stderr, "Error: execve failed.\n");
			exit(0);
		}
	}
	while (wait(&pid) > 0);

	return 1;
}

int piping(char *cmd) {
	
	char *token[27];
	int num_tokens = tokenize_string(token, cmd);

	int delim_1 = -1;
	int delim_2 = -1;
	int i = 0;

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

	printf("d1: %d d2: %d\n", delim_1, delim_2);

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
		delim_2 = num_tokens;
	
	int j;
	
	printf("multi: %d delim2: %d delim1: %d\n", multi, delim_2, delim_1);
	
	int s1 = (delim_1-1 < MAX_NUM_ARGS+1) ? delim_1-1 : MAX_NUM_ARGS+1;
	char *token_1[s1+1];
	for(j = 1; j < delim_1; j++) {
		token_1[j-1] = token[j];
	}
	token_1[j-1] = 0;

	int s2 = (delim_2-delim_1-1 < MAX_NUM_ARGS+1) ? delim_2-delim_1-1 : MAX_NUM_ARGS+1;
	char *token_2[s2+1];
	i = 0;
	for(j = delim_1+1; j < delim_2; j++)
		token_2[i++] = token[j];
	token[j-1] = 0;
	
	/*
	char *token_3[(multi==2) ? num_tokens-delim_2+1 : 1];
	i = 0;
	if(multi == 2){
		for(j = delim_2+1; j < num_tokens; j++)
			token_3[i++] = token[j];
	}else
		token_3[0] = NULL;
	*/
/*
	int z = 0;
	printf("token_1[%lu]: ",*(&token_1+1)-token_1);
	while(z < *(&token_1+1)-token_1){
		printf("%s ", token_1[z]);
		z++;
	}
	printf("\n");
	
	z = 0;
	printf("token_2[%lu]: ",*(&token_2+1)-token_2);
	while(z < *(&token_2+1)-token_2){
		printf("%s ", token_2[z]);
		z++;
	}
	printf("\n");
*/
	/*
	z = 0;
	printf("token_3[%lu]: ",*(&token_3+1)-token_3);
	while(z < *(&token_1+1)-token_1){
		printf("%s ", token_3[z]);
		z++;
	}
	printf("\n");
	*/

	char binary_1[MAX_LINE_LENGTH];
	char binary_2[MAX_LINE_LENGTH];
	//char binary_3[MAX_LINE_LENGTH];

	int found_1 = find_path(token_1[0], binary_1);
	int found_2 = find_path(token_2[0], binary_2);
	//find_path(token_3[0], binary_3);

	printf("path1: %s\n", binary_1);
	printf("path2: %s\n", binary_2);
	
	if(!found_1){
		fprintf(stderr, "Error: %s - Invalid Command\n", token_1[0]);
		return 0;
	}
	if(!found_2) {
		fprintf(stderr, "Error: %s - Invalid Command\n", token_2[0]);
		return 0;
	}

	char *envp[] = { 0 };
	int status;
	int pid1, pid2;
	int fd[2];

	token_1[0] = binary_1;
	token_2[0] = binary_2;

	pipe(fd);

	if((pid1 = fork()) == 0) {
		dup2(fd[1], 1);
		close(fd[0]);
		
		if(execve(token_1[0], token_1, envp) == -1) {
			fprintf(stderr, "Error: execve on cmd 1 failed.\n");
			exit(0);	
		}
	}

	if((pid2 = fork()) == 0) {
		dup2(fd[1], 1);
        close(fd[0]);
		
		fprintf(stderr, "arg: %s\n", token_2[1]);

		if(execve(token_2[0], token_2, envp) == -1) {
			fprintf(stderr, "Error: execve on cmd 2 failed.\n");
			exit(0);	
		}
	}
	

	close(fd[0]);
    close(fd[1]);
	
	printf("1 waiting...");
    waitpid(pid1, &status, 0);
	printf("1 done.");

	printf("2 waiting...");
    waitpid(pid2, &status, 0); 
	printf("2 done.");
	
	return 1;
}

int output_redirect(char *cmd) {
	return 0;
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
		else
			run_cmd(input);
	}
	return 0;
}
