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

#define MAX_NUM_ARGS 10 // 7
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

int find_path(char *binary, char *fullpath) {
	return 1;
}

int tokenize_string(char **token, char *cmd) {
	char *t;
	int num_tokens = 0;

	t = strtok(cmd, " ");
	while(t != NULL && num_tokens < MAX_NUM_ARGS) {
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

int piping(char *cmd) {
	
	char *token[MAX_NUM_ARGS];
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

	if(delim_1 == -1) {
		fprintf(stderr, "PP missing '->'. Usage: PP <command> -> <command>\n");
		return 0;
	}
	if(delim_1 == 2) {
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
	
	char *token_1[delim_1-1];
	for(j = 1; j < delim_1; j++) {
		token_1[j-1] = token[j];
	}
	
	char *token_2[delim_2-delim_1];
	i = 0;
	for(j = delim_1+1; j < delim_2; j++)
		token_2[i++] = token[j];
	
	char *token_3[num_tokens-delim_2];
	i = 0;
	if(multi == 2){
		for(j = delim_2+1; j < num_tokens; j++)
			token_3[i++] = token[j];
	}
	
	char binary_1[MAX_LINE_LENGTH];
	char binary_2[MAX_LINE_LENGTH];
	char binary_3[MAX_LINE_LENGTH];

	int find_path(token_1[0], binary_1);
	int find_path(token_2[0], binary_2);
	int find_path(token_3[0], binary_3);
		
	
	return 0;
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
		if(strncmp(input, "PP", 2) == 0)
			piping(input);
		if(strncmp(input, "OR", 2) == 0)
			output_redirect(input);
	}
	return 0;
}
