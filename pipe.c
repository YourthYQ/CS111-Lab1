#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <program1> [program2 ...]\n", argv[0]);
        return EINVAL;  // Invalid argument
    }

    int num_commands = argc - 1;
    int i;
    int pipe_fds[2 * (num_commands - 1)];  // Array to hold the file descriptors for all pipes

    // Create all necessary pipes
    for (i = 0; i < num_commands - 1; i++) {
        if (pipe(pipe_fds + i * 2) < 0) {
            perror("pipe");
            exit(EXIT_FAILURE);
        }
    }

    int status;
    for (i = 0; i < num_commands; i++) {
        pid_t pid = fork();
        if (pid == 0) {  // Child process
            // If not the first command, get input from the previous pipe
            if (i > 0) {
                if (dup2(pipe_fds[(i - 1) * 2], STDIN_FILENO) < 0) {
                    perror("dup2");
                    exit(EXIT_FAILURE);
                }
            }

            // If not the last command, output to the next pipe
            if (i < num_commands - 1) {
                if (dup2(pipe_fds[i * 2 + 1], STDOUT_FILENO) < 0) {
                    perror("dup2");
                    exit(EXIT_FAILURE);
                }
            }

            // Close all pipe file descriptors in the child process
            for (int j = 0; j < 2 * (num_commands - 1); j++) {
                close(pipe_fds[j]);
            }

            execlp(argv[i + 1], argv[i + 1], (char *)NULL);
            perror("execlp");  // execlp only returns on error
            exit(errno);  // Exit with the errno set by execlp
        } else if (pid < 0) {
            perror("fork");
            exit(EXIT_FAILURE);
        }
    }

    // Parent closes all pipe file descriptors
    for (i = 0; i < 2 * (num_commands - 1); i++) {
        close(pipe_fds[i]);
    }

    // Parent waits for all child processes to finish and checks for any errors
    int exit_status = 0;
    for (i = 0; i < num_commands; i++) {
        wait(&status);
        if (WIFEXITED(status) && WEXITSTATUS(status) != 0) {
            exit_status = WEXITSTATUS(status);
        }
    }

    return exit_status;
}
