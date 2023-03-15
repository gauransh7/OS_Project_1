#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define NUM_THREADS 4
#define BARRIER_COUNT 3

// Define the barrier semaphore and count variables
sem_t barrier_sem;
int count = 0;

// Define the thread IDs
pthread_t threads[NUM_THREADS];

// Define the thread functions
void *thread_function(void *arg) {
    int id = *(int*)arg;
    int i;
    for (i = 0; i < BARRIER_COUNT; i++) {
        // Perform some work
        printf("Thread %d is doing work\n", id);
        sleep(1);

        // Enter the barrier
        sem_wait(&barrier_sem);
        count++;
        sem_post(&barrier_sem);
        
        // Wait for all threads to reach the barrier
        while (count < (i + 1)*NUM_THREADS) {
            sleep(1);
        }
        
        // Exit the barrier
        sem_wait(&barrier_sem);
        barrier_count++;
        sem_post(&barrier_sem);
        
        // Wait for all threads to exit the barrier
        while (barrier_count < (i + 1)*NUM_THREADS) {
            sleep(1);
        }
    }
    printf("Thread %d has finished\n", id);
    pthread_exit(NULL);
}

// Define the main function
int main(int argc, char *argv[]) {
    int i, rc;
    int thread_ids[NUM_THREADS] = {0, 1, 2, 3};
    
    // Initialize the barrier semaphore
    sem_init(&barrier_sem, 0, 1);
    
    // Create the threads
    for (i = 0; i < NUM_THREADS; i++) {
        pthread_create(&threads[i], NULL, thread_function, &thread_ids[i]);
    }
    
    // Wait for the threads to finish
    for (i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }
    
    // Destroy the barrier semaphore
    sem_destroy(&barrier_sem);
    
    return 0;
}
