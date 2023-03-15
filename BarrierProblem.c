#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <stdatomic.h>
#include <stdbool.h>

#define NUM_THREADS 4
#define BARRIER_COUNT 3


// Define Semaphore struct with counter and mutex
typedef struct{
    atomic_int counter;  // counter variable to represent the number of resources available
    atomic_flag lock; // lock variable to synchronize access to the counter variable
} Semaphore;

// Initialize Semaphore
void sem_init(Semaphore *s, int counter) {
    // Initialize counter variable with given counter
    atomic_init(&s->counter, counter);
    // Clear the lock variable to ensure Semaphore is initially unlocked
    atomic_flag_clear(&s->lock);
}

// Wait operation on Semaphore
void sem_wait(Semaphore *s) {
    // Spin until we can acquire the lock
    while (atomic_flag_test_and_set(&s->lock));
    // Spin until the counter variable is positive
    while (atomic_load(&s->counter) <= 0);
    // Decrement the counter variable
    atomic_fetch_sub(&s->counter, 1);
    // Release the lock
    atomic_flag_clear(&s->lock);
}

// Post operation on Semaphore
void sem_post(Semaphore *s) {
    // Increment the counter variable
    atomic_fetch_add(&s->counter, 1);
}

// Destroy Semaphore
void sem_destroy(Semaphore *s) {
    // Release all resources (i.e., increment counter variable until it reaches its initial counter)
    for (int i = 0; i < atomic_load(&s->counter); i++) {
        sem_post(s);
    }
}

// Define the barrier semaphore and count variables
// int barrier_sem;
int count = 0;
Semaphore barrier_sem;

// Define the thread IDs
pthread_t threads[NUM_THREADS];


// Define the thread functions
void *thread_function(void *arg) {
    int id = *(int*)arg;
    int i;
    for (i = 0; i < BARRIER_COUNT; i++) { //each thread has to execute BARRIER_COUNT times
        // Perform some work
        printf("Thread %d is doing work\n", id);
        sleep(1);

        // Enter the barrier
        sem_wait(&barrier_sem);
        count++;
        sem_post(&barrier_sem);

        // Wait for all threads to reach the barrier
        while (count < (i + 1)*NUM_THREADS) { //count should be = (i+1)*NUM_THREADS if all threads have executed once in the i+1th operation
            sleep(1); // so if thread increments its count and all threads have not completed it will wait for all to cross the i+1th barrier
        }
    }
    printf("Thread %d has finished\n", id);
    pthread_exit(NULL);
}


int main(int argc, char *argv[]) {
    int i, rc;
    int thread_ids[NUM_THREADS] = {0, 1, 2, 3};

    // Initialize the barrier semaphore
    sem_init(&barrier_sem,1);

    // Create the threads
    for (i = 0; i < NUM_THREADS; i++) {
        pthread_create(&threads[i], NULL, thread_function, &thread_ids[i]);
    }
    
    // Wait for the threads to finish
    for (i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    sem_destroy(&barrier_sem);

    return 0;
}
