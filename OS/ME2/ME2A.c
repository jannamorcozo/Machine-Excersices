#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/**
 *  This program continuously creates new child processes in an infinite loop.
 *  For each iteration case:
 *  (-1) If fork() fails, an error message is displayed and the program exits.
 *  (0) The child process prints its PID and parent PID.
 *  (1) The parent process prints its PID and parent PID.
 */
int main() {
    while (1) { // infinite loop
        pid_t pid = fork(); // start fork process

        switch(pid) {
            case -1: // Fork Error
                perror("Fork");
                exit(1);

            case 0: // Child Process
                printf("[CHILD]: PID %d, PPID %d\n", getpid(), getppid());
                exit(0);
                
            default: // Parent Process
                printf("[PARENT]: PID %d, PPID %d\n", getpid(), getppid());
        }
    }
    return 0;
}
