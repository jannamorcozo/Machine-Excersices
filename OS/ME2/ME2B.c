#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

/**
 *  This program forks a child process:
 *  > In the child process, execl() is used to replace the process image
 *   with the external program "./exec_code". If execl() fails, an error
 *   message is displayed and the child exits with failure.
 *  > In the parent process, a message is printed showing its PID and the child's PID.
 *   The parent then waits for the child to finish execution using wait().
 */
int main() {
    pid_t pid = fork();

    switch(pid) {
        case -1: // Fork error
            perror("fork");
            exit(1);

        case 0: // Child process
            execl("./exec_code", "exec_code", NULL);
            perror("exec");
            exit(1);

        default: { // Parent process
            printf("[PARENT]: PID %d, waits for child with PID %d\n", getpid(), pid);

            int wstatus; // stores encoded info about child termination
            pid_t child_pid = wait(&wstatus); // suspends parent process and store pid of child

            if (WIFEXITED(wstatus)) // returns true if child exited normally
                printf("[PARENT]: Child with PID %d finished and unloaded.\n", child_pid);
            else if (WIFSIGNALED(wstatus)) // returns true if child is terminated by a signal
                printf("[PARENT]: Child with PID %d was killed by signal %d.\n", child_pid, WTERMSIG(wstatus));
            else // Error message when neither of previous condition is true
                printf("[PARENT]: Child with PID %d terminated abnormally.\n", child_pid);
        }
    }
    return 0;
}
