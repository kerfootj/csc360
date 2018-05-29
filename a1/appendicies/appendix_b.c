/*
 * Appendix B program.
 * CSC 360, Summer 2017
 *
 * In this example, fork & execve are used to create a child process
 * that runs the "ls" command with one argument given to the "ls" command
 * (i.e., "-1" which lists all of the files in a single column).
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    char *args[] = { "/bin/ls", "-1", 0 };
    char *envp[] = { 0 };
    int pid;
    int status;

    if ((pid = fork()) == 0) {
        printf("child: about to start...\n");
        execve(args[0], args, envp);
        printf("child: SHOULDN'T BE HERE.\n");
    }

    printf("parent: waiting for child to finish...\n");
    while (wait(&status) > 0) {
        printf("parent: child is finished.\n");
    } 
}
