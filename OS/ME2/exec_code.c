#include <stdio.h>
#include <unistd.h>

int main () {
    printf("[CHILD]: PID %d, starts counting:\n", getpid());

    // count loop from 1-10
    for (int i = 1; i <= 10; i++) {
        printf("[CHILD]: i = %d\n", i);
    }
    
    return 0;
}