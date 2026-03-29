#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>
#include <sys/wait.h>

/**
 *  This program converts the case of a character:
 *  > If its lowercase, it becomes uppercase
 *  > If its uppercase, it becomes lowercase
 *  > If its non-alphabetic it is ignored
 */
void reverse_case(char *str) {
    for(int i = 0; i < strlen(str); i++) {
        if (islower(str[i]))
            str[i] = toupper(str[i]);
        else if (isupper(str[i]))
            str[i] = tolower(str[i]);
    }
}

int main() {
    int pipe1_fd[2];        // Parent to Child Pipe
    int pipe2_fd[2];        // Child  to Parent pipe
    const int size = 100;   // constant size
    char message[size];     // stores user input message
    char buffer[size];      // used for data transfer in pipes

    // creation and error checking of pipe channel
    if (pipe(pipe1_fd) == -1 || pipe(pipe2_fd) == -1) {
        perror("pipe");
        exit(1);
    }

    pid_t pid = fork(); // start process

    switch(pid) {
        case -1: // Fork Error 
            perror("fork");
            exit(1);

        case 0: // Child Process
            close(pipe1_fd[1]); // close write end of parent
            close(pipe2_fd[0]); // close read end of child

            read(pipe1_fd[0], buffer, size); // read message from parent
            printf("CHILD(%d): Recieved message\n", getpid());

            printf("CHILD(%d): Reversing the case of the string and sending to Parent\n", getpid());
            reverse_case(buffer); // convert the string cases
            write(pipe2_fd[1], buffer, size); // write converted message to parent

            close(pipe1_fd[0]); // close read end of parent
            close(pipe2_fd[1]); // close write end of child
            exit(0);
        
        default: // Parent Process
            close(pipe1_fd[0]); // close read end of parent
            close(pipe2_fd[1]); // close write end of child

            printf("Input string message: ");
            fgets(message, size, stdin); // takes user input messages
            message[strcspn(message, "\n")] = '\0'; // removes the newline character in the message

            printf("PARENT(%d): Sending [%s] to Child\n", getpid(), message);
            write(pipe1_fd[1], message, size); // write message to child

            read(pipe2_fd[0], buffer, size); // read converted message from child
            printf("PARENT(%d): Received [%s] from Child\n", getpid(), buffer);

            close(pipe1_fd[1]); // close write end of parent
            close(pipe2_fd[0]); // close read end of child

            wait(NULL); // wait and check if child process is done
    }
    return 0;
}