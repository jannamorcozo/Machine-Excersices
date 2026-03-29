#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/**
 *  This function asks the user to provide a source file name and a destination
 *  file name. If the user enters "quit", the program terminates immediately.  
 *  The source file is opened in read mode ("r"), and the destination file is
 *  opened in write mode ("w"), creating or overwriting the file if it exists.
 *
 *  @param source      Character array to store the source file name.
 *  @param destination Character array to store the destination file name.
 *  @param src         Pointer to a FILE pointer that will be assigned the opened source file.
 *  @param dest        Pointer to a FILE pointer that will be assigned the opened destination file.
 *
 *  @return 1 if both files are successfully opened, 0 if opening the source file fails.
 */
int file_input(char source[], char destination[], FILE **src, FILE **dest) {
    printf("\nEnter the source file name (or 'quit'): ");
    scanf("%99s", source);

    if (strcmp(source, "quit") == 0) {
        printf("Quitting program...\n");
        exit(0);
    } // handles quit option

    // handles source file opening
    *src = fopen(source, "r");
    if (*src == NULL) {
        perror("Source File Error");
        return 0;
    }

    printf("Enter the destination file name (or 'quit'): ");
    scanf("%99s", destination);

    if (strcmp(destination, "quit") == 0) {
        printf("Quitting program...\n");
        fclose(*src);
        exit(0);
    } // handles quit option

    //handles destination file opening/creation
    *dest = fopen(destination, "w");
    return 1;
}

int main() {
    char source[100], destination[100];     // Name handler of the files
    FILE *src, *dest;                       // FILE pointer for file management operations

    while (1) {
        if (file_input(source, destination, &src, &dest))
            break;  // loop until both files are valid
    }

    int counter;
    if ((counter = fgetc(src)) == EOF){
            printf("Souce file is empty.\n");
            exit(1);
    } // handles empty source file with fgetc() return value

    do {
        fputc(counter, dest); // writes each character to destination file
    } while ((counter = fgetc(src)) != EOF); // loop until EOF

    fclose(src);
    fclose(dest);
    printf("File copied successfully.\n");
    return 0;
}
