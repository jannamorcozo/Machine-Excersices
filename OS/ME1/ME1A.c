#include <stdio.h> 
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

/**
 *  This function prompts the user to enter a source file and a destination file.  
 *  If the user types "quit", the program will terminate.  
 *  It attempts to open the source file in read-only mode and the destination
 *  file in write mode (creating/truncating it if necessary).
 *
 *  @param source      Character array to store the source file name.
 *  @param destination Character array to store the destination file name.
 *  @param src_fd      Pointer to an integer where the source file descriptor will be stored.
 *  @param dest_fd     Pointer to an integer where the destination file descriptor will be stored.
 *
 *  @return 1 if both files are successfully opened, 0 if opening the source file fails.
 */
int file_input(char source[], char destination[], int *src_fd, int *dest_fd) {
    printf("\nEnter the source file name (or 'quit'): ");
    scanf("%99s", source);

    if (strcmp(source, "quit") == 0) {
        printf("Quiting program...");
        exit(0);
    } // handles quit option

    // handles source file opening
    *src_fd = open(source, O_RDONLY);
    if (*src_fd < 0) {
        perror("Source File Error");
        return 0;
    }

    printf("Enter the destination file name (or 'quit'): ");
    scanf("%99s", destination);

    if (strcmp(destination, "quit") == 0) {
        printf("Quiting program...");
        exit(0);
    } // handles quit option

    //handles destination file opening/creation
    *dest_fd = open(destination, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    return 1;
}

int main() {
    const int buffer_size = 4096;           // 4KB (typical system page size, efficient for I/O)
    char buffer[buffer_size];               // Stores the data of file 
    char source[100], destination[100];     // Name handler of the files
    int src_fd, dest_fd;                    // File Descriptor : integer handle used to access an open file
    int bytes_len;                          // Number of bytes read from or written to the file

    while (1) {
        if (file_input(source, destination, &src_fd, &dest_fd))
            break;  // loop until both files are valid
    }

    if ((bytes_len = read(src_fd, buffer, buffer_size)) == 0) {
            printf("Souce file is empty.\n");
            exit(1);
    } // handles empty source file with read() return value

    do {
        
        if (write(dest_fd, buffer, bytes_len) != bytes_len) {
            perror("Write Error");
            close(src_fd);
            close(dest_fd);
            exit(1);
        } // handles write() error
    } while ((bytes_len = read(src_fd, buffer, buffer_size)) > 0); // loop until EOF

    if (bytes_len < 0) 
        perror("Read Error"); // handles read() error

    close(src_fd);
    close(dest_fd);
    printf("File copied successfully.\n");
    return 0;
}
