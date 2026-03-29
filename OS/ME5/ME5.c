#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

/**
 * Prints the error message, sets errno, and exits the program.
 *
 * @param en  Error number
 * @param msg Message to display
 */
#define handle_error_en(en, msg) \
    do { errno = en; perror(msg); exit(EXIT_FAILURE); } while (0)

/**
 * This function is called when a worker thread is cancelled. 
 * It can be used to free resources or perform any necessary cleanup.
 *
 * @param arg Pointer to a string message describing the cleanup
 */
void cleanup_handler(void *arg) {
    printf("%s\n", (char *)arg);
}

/**
 * Worker thread function with asynchronous cancellation.
 * 
 * This thread runs an infinite loop printing numbers and 
 * can be cancelled at any time. Cleanup handler will be invoked
 * on cancellation.
 *
 * @param arg Unused
 * @return NULL
 */
void *worker_async(void *arg) {
    // pushes routines on top stack to handle cleaning resources
    pthread_cleanup_push(cleanup_handler, "Asynchronous Worker Cleanup.");

    // initialize asynchronous cancel type
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);

    int i = 0;
    while (1) {
        printf("\tThread counter %d\n", i++);
        usleep(500000); // sleep 0.5 sec
    }
    
    // pops routines on top stack to handle cleaning resources
    // will only happen in normal execution
    pthread_cleanup_pop(1);
    return NULL;
}

/**
 * Worker thread function with deferred cancellation.
 *
 * This thread runs an infinite loop printing numbers and 
 * responds to cancellation only at defined cancellation points.
 * Cleanup handler will be invoked on cancellation.
 *
 * @param arg Unused
 * @return NULL
 */
void *worker_deferred(void *arg) {
    // pushes routines on top stack to handle cleaning resources
    pthread_cleanup_push(cleanup_handler, "Deferred Worker Cleanup.");

    // initialize deferred cancel type
    pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, NULL);

    int i = 0;
    while (1) {
        printf("\tThread counter %d\n", i++);
        usleep(500000); // sleep 0.5 sec

        pthread_testcancel(); // regularly checks safety points in thread process
    }

    // pops routines on top stack to handle cleaning resources
    // will only happen in normal execution
    pthread_cleanup_pop(1);
    return NULL;
}

/** 
 * Main Function:
 * 
 * Creates two worker threads: one with asynchronous cancellation 
 * and one with deferred cancellation. Each worker runs for a 
 * few seconds and is then cancelled. Cleanup handlers demonstrate
 * proper resource management.
 *
 * @return int Program exit status
 */
int main() {
    pthread_t asynch_tid, deferred_tid;

    // Scenario 1
    printf("Creating ASYNCHRONOUS worker thread...\n");
    if (pthread_create(&asynch_tid, NULL, worker_async, NULL) != 0)
        handle_error_en(errno, "pthread_create async");

    sleep(3);

    printf("\nCancelling ASYNCHRONOUS worker thread...\n");
    if (pthread_cancel(asynch_tid) != 0)
        handle_error_en(errno, "pthread_cancel async");

    pthread_join(asynch_tid, NULL);
    printf("ASYNCHRONOUS worker terminated\n\n");

    //Scenario 2
    printf("Creating DEFERRED worker thread...\n");
    if (pthread_create(&deferred_tid, NULL, worker_deferred, NULL) != 0)
        handle_error_en(errno, "pthread_create deferred");

    sleep(3);

    printf("\nCancelling DEFERRED worker thread...\n");
    if (pthread_cancel(deferred_tid) != 0)
        handle_error_en(errno, "pthread_cancel deferred");

    pthread_join(deferred_tid, NULL);
    printf("DEFERRED worker terminated\n");

    return 0;
}