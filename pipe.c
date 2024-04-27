#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>

int main(int argc, char *argv[]) {

	// Check if No argument at the command line
    if (argc < 2) {
        fprintf(stderr, "Usage: %s [program1] [program2]...\n", argv[0]);
        return EINVAL; // Invalid argument
    }

    int num_commands = argc - 1;
    int i;

	// Create an array to hold the file descriptors for all pipes
    int pipe_fds[2 * (num_commands - 1)];

    // Create all necessary pipes
    for (i = 0; i < num_commands - 1; i++) {
        if (pipe(pipe_fds + i * 2) < 0) {
            perror("Failed to create pipe");
            exit(EXIT_FAILURE);
        }
    }

    int status;
    for (i = 0; i < num_commands; i++) {

        pid_t pid = fork();
        if (pid == 0) {  // Child process

            // If not the first command, redirect stdin from the read end of the previous pipe
            if (i > 0) {
                if (dup2(pipe_fds[(i - 1) * 2], STDIN_FILENO) < 0) {
                    perror("Failed to set up stdin");
                    exit(EXIT_FAILURE);
                }
            }

            // If not the last command, redirect stdout to the write end of the current pipe
            if (i < num_commands - 1) {
                if (dup2(pipe_fds[i * 2 + 1], STDOUT_FILENO) < 0) {
                    perror("Failed to set up stdout");
                    exit(EXIT_FAILURE);
                }
            }

            // Close all pipe file descriptors in the child process
            for (int j = 0; j < 2 * (num_commands - 1); j++) {
                close(pipe_fds[j]);
            }

			// Execute the command using execlp: if execlp fails, output error message and exit
            execlp(argv[i + 1], argv[i + 1], (char *)NULL);
            perror("Wrong argument");
            exit(errno); // Exit with the errno set by execlp

        } else if (pid < 0) {
			// Output an error message and exit if process creation (fork) fails
            perror("Failed to create a new process");
            exit(EXIT_FAILURE);
        }

    }

    // Parent process closes all pipe file descriptors after forking
    for (i = 0; i < 2 * (num_commands - 1); i++) {
        close(pipe_fds[i]);
    }

    // Parent waits for all child processes to finish and checks for any errors
    int exit_status = 0;
    for (i = 0; i < num_commands; i++) {
        wait(&status);
		// Check if any child process exited with a status other than 0 (indicating an error)
        if (WIFEXITED(status) && WEXITSTATUS(status) != 0) {
            exit_status = WEXITSTATUS(status);
        }
    }

    return exit_status;
}
