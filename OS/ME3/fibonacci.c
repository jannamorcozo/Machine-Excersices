#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/stat.h>

#define MAX_SEQUENCE 10

// gcc fib_shm.c -o fib_shm
// ./fib_shm 8

typedef struct {
    long fib_sequence[MAX_SEQUENCE];
    int sequence_size;
} shared_data;

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <N>\n", argv[0]);
        exit(1);
    }

    int N = atoi(argv[1]);
    if (N <= 0 || N > MAX_SEQUENCE) {
        fprintf(stderr, "N must be between 1 and %d\n", MAX_SEQUENCE);
        exit(1);
    }

    // Create shared memory
    int seg_id = shmget(IPC_PRIVATE, sizeof(shared_data), IPC_CREAT | S_IRUSR | S_IWUSR);
    if (seg_id < 0) {
        perror("shmget");
        exit(1);
    }

    // Attach shared memory
    shared_data *sd = (shared_data*) shmat(seg_id, NULL, 0);
    if (sd == (void*) -1) {
        perror("shmat");
        exit(1);
    }

    sd->sequence_size = N;

    pid_t pid = fork();

    if (pid < 0) {
        perror("fork");
        exit(1);
    }

    if (pid == 0) { // Child process: generate Fibonacci
        if (N >= 1) sd->fib_sequence[0] = 0;
        if (N >= 2) sd->fib_sequence[1] = 1;
        for (int i = 2; i < N; i++) {
            sd->fib_sequence[i] = sd->fib_sequence[i-1] + sd->fib_sequence[i-2];
        }
        shmdt(sd); // Detach child
        exit(0);
    } else { // Parent process: print Fibonacci
        wait(NULL);
        printf("Fibonacci sequence (%d terms): ", sd->sequence_size);
        for (int i = 0; i < sd->sequence_size; i++) {
            printf("%ld ", sd->fib_sequence[i]);
        }
        printf("\n");

        shmdt(sd);                  // Detach parent
        shmctl(seg_id, IPC_RMID, NULL); // Remove shared memory
    }

    return 0;
}