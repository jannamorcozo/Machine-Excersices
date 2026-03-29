#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

// Global matrices
int **matrix_A, **matrix_B, **matrix_product;
int row_A, col_A, row_B, col_B;

typedef struct {
    int start_row;      // starting row index for this thread
    int end_row;        // ending row index (exclusive) for this thread
    int thread_id;      // thread identifier
} ThreadData;

/**
 * Prints a given matrix.
 *
 * @param M The matrix to print
 * @param rows Number of rows
 * @param cols Number of columns
 * @param name Name of the matrix (e.g., 'A' or 'B')
 */
void printInputMatrix(int **M, int rows, int cols, char name) {
    printf("\nMatrix %c (%dx%d):\n", name, rows, cols);
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            printf("%4d ", M[i][j]);
        }
        printf("\n");
    }
}

// Prints product matrix
void printProductMatrix(int **M, int rows, int cols) {
    printf("\nProduct Matrix (%dx%d):\n", rows, cols);
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            printf("%4d ", M[i][j]);
        }
        printf("\n");
    }
}

/**
 * Thread function to perform matrix multiplication for a subset of rows.
 *
 * Each thread multiplies rows from matrix_A with matrix_B and stores the result
 * in matrix_product. The execution wall time is recorded in the ThreadData struct.
 *
 * @param arg Pointer to a ThreadData structure
 * @return NULL
 */
void *multiply(void *arg) {
    ThreadData *data = (ThreadData *)arg;   // allows each thread to know its data

    // multiply assigned rows of matrix_A by matrix_B
    for (int i = data->start_row; i < data->end_row; i++) {
        for (int j = 0; j < col_B; j++) {
            matrix_product[i][j] = 0;
            for (int k = 0; k < col_A; k++) { // col_A == row_B
                matrix_product[i][j] += matrix_A[i][k] * matrix_B[k][j];
            }
        }
    }

    pthread_exit(NULL);
}

/**
 * Main function:
 *
 * - Prompts user for matrix sizes and number of threads.
 * - Allocates and populates matrices with random integers.
 * - Creates threads for matrix multiplication.
 * - Joins threads and prints per-thread wall times.
 * - Prints final product matrix and overall elapsed time.
 *
 * @return int Program exit status
 */
int main() {
    int thread_size;

    while (1) {
        printf("Enter rows and cols for Matrix A (row_A col_A): ");
        scanf("%d %d", &row_A, &col_A);

        printf("Enter rows and cols for Matrix B (row_B col_B): ");
        scanf("%d %d", &row_B, &col_B);

        if (col_A != row_B) {
            printf("\nError: Matrix sizes incompatible!\n");
            printf("Tip: Matrix A column size should be equal to Matrix B row size.\n\n");
        }
        else break;
    }

    printf("Enter thread_size: ");
    scanf("%d", &thread_size);

    // allocate matrices
    matrix_A = malloc(row_A * sizeof(int *));
    for (int i = 0; i < row_A; i++) 
        matrix_A[i] = malloc(col_A * sizeof(int));

    matrix_B = malloc(row_B * sizeof(int *));
    for (int i = 0; i < row_B; i++) 
        matrix_B[i] = malloc(col_B * sizeof(int));

    matrix_product = malloc(row_A * sizeof(int *));
    for (int i = 0; i < row_A; i++) 
        matrix_product[i] = malloc(col_B * sizeof(int));

    // fill with random values
    srand(time(NULL));
    for (int i = 0; i < row_A; i++)
        for (int j = 0; j < col_A; j++) matrix_A[i][j] = rand() % 10;

    for (int i = 0; i < row_B; i++)
        for (int j = 0; j < col_B; j++) matrix_B[i][j] = rand() % 10;

    // print inputs
    printInputMatrix(matrix_A, row_A, col_A, 'A');
    printInputMatrix(matrix_B, row_B, col_B, 'B');

    pthread_t threads[thread_size]; // keeps track of thread handles (for pthread_join)
    ThreadData data[thread_size];   // keeps thread-specific parameters

    // Determine rows per thread for even distribution
    int rows_per_thread = row_A / thread_size;
    int remainder = row_A % thread_size; // for uneven division
    int start = 0;

    struct timespec t_start, t_end;
    clock_gettime(CLOCK_MONOTONIC, &t_start); // start overall timing

    for (int t = 0; t < thread_size; t++) {
        int extra = (t < remainder) ? 1 : 0; // Distribute remaining rows
        data[t].start_row = start;
        data[t].end_row = start + rows_per_thread + extra;
        data[t].thread_id = t + 1;
        start = data[t].end_row;

        pthread_create(&threads[t], NULL, multiply, &data[t]); // launch thread
    }

    // join threads and report execution times
    for (int t = 0; t < thread_size; t++) {
        pthread_join(threads[t], NULL);
    }

    // stop overall timing
    clock_gettime(CLOCK_MONOTONIC, &t_end);
    double exec_time = (t_end.tv_sec - t_start.tv_sec) + (t_end.tv_nsec - t_start.tv_nsec) / 1e9;

    // print product matrix and summary
    printProductMatrix(matrix_product, row_A, col_B);
    printf("\nTotal computed execution time: %lf sec\n", exec_time);

    return 0;
}