
/******************************* PROTOTYPES *******************************/
int change_directory(char **token, int num_tokens);
int make_directory(char **token, int num_tokens);

/*************************** IMPLEMENTATION *****************************/

int change_directory(char **token, int num_tokens) {

	if(num_tokens != 2) {
		fprintf(stderr, "Invalid Syntax. Usage: CD <directory>\n");
		return 0;
	}

	char cwdir[256];
	int status;

	if(token[1][0] != '/') {

		getcwd(cwdir, sizeof(cwdir));
		strcat(cwdir, "/");
		strcat(cwdir, token[1]);
		status = chdir(cwdir);

	} else {
		status = chdir(token[1]);
	}

	if(status != 0) 
		fprintf(stderr, "Error: couldn't change directory\n");
	return status;
}

int make_directory(char **token, int num_tokens) {

	if(num_tokens != 2) {
		fprintf(stderr, "Invalid Syntax. Usage: MKDIR <directory>\n");
		return 1;
	}

	int status;

	status = mkdir(token[1], S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);

	if(status != 0) 
		fprintf(stderr, "Error: couldn't make directory %s\n", token[1]);
	return status;
}