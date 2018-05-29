/*
 * Appendix C program.
 * CSC 360, Summer 2017
 *
 * Like the code in Appendix B, this creates a child running -- in effect -- the * command "ls -1". This time, however, the output from the command is not
 * output to the console but rather is redirected to a file named
 * "output.txt". That is, the result of this program would be the same as if
 * we had run "ls -1 > output.txt" in bash.
 *
 * Note the three arguments used for open().
 * 
 * Also note the use of dup2(). This system call is perhaps easier to use
 * than dup(). For example, "dup2(fd, 1)" means "force file descriptor 
 * corresponding to stream 1 -- stdout -- to now be connected to the file
 * to which "fd" refers". Remember -- the output is now stored in a file
 * named "output.txt", which means you will *not* see the directory listing
 * appearing on the console when the program is running.
 * 
 * Finally note that the end of the program the parent waits for the
 * child by explicitly referring to the child's pid (i.e., waitpid()
 * instead of the loop involving wait().)
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int main(int argc, char *argv[]) {
    char *args[] = { "/bin/ls", "-1", 0 };
    char *envp[] = { 0 };
    int pid, fd;
    int status;

    if ((pid = fork()) == 0) {
        printf("child: about to start...\n");
      
        fd = open("output.txt", O_CREAT|O_RDWR, S_IRUSR|S_IWUSR);
        if (fd == -1) {
            fprintf(stderr, "cannot open output.txt for writing\n");
            exit(1);
        }
        dup2(fd, 1);
        dup2(fd, 2); 
        execve(args[0], args, envp);
        printf("child: SHOULDN'T BE HERE.\n");
    }

    printf("parent: waiting for child to finish...\n");
    waitpid(pid, &status, 0);
    printf("parent: child is finished.\n");
}
