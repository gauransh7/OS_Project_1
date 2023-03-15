#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

// Define the monitor data structure
typedef struct {
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    int is_busy;
} Monitor;

// Define the monitor functions
void monitor_init(Monitor *m) {
    pthread_mutex_init(&m->mutex, NULL);
    pthread_cond_init(&m->cond, NULL);
    m->is_busy = 0;
}

void monitor_enter(Monitor *m) {
    pthread_mutex_lock(&m->mutex);
    while (m->is_busy) {
        pthread_cond_wait(&m->cond, &m->mutex);
    }
    m->is_busy = 1;
    pthread_mutex_unlock(&m->mutex);
}

void monitor_exit(Monitor *m) {
    pthread_mutex_lock(&m->mutex);
    m->is_busy = 0;
    pthread_cond_signal(&m->cond);
    pthread_mutex_unlock(&m->mutex);
}

// Define the thread function
void *thread_function(void *arg) {
    Monitor *m = (Monitor*)arg;
    
    // Enter the monitor and access the shared resource
    monitor_enter(m);
    printf("Thread %lu is accessing the shared resource\n", pthread_self());
    sleep(1);
    monitor_exit(m);
    
    // Exit the thread
    printf("Thread %lu has finished\n", pthread_self());
    pthread_exit(NULL);
}

// Define the main function
int main(int argc, char *argv[]) {
    int i, rc;
    pthread_t threads[5];
    Monitor m;
    
    // Initialize the monitor
    monitor_init(&m);
    
    // Create the threads
    for (i = 0; i < 5; i++) {
        rc = pthread_create(&threads[i], NULL, thread_function, &m);
        if (rc != 0) {
            printf("Error creating thread %d\n", i);
            exit(1);
        }
    }
    
    // Wait for the threads to finish
    for (i = 0; i < 5; i++) {
        rc = pthread_join(threads[i], NULL);
        if (rc != 0) {
            printf("Error joining thread %d\n", i);
            exit(1);
        }
    }
    
    // Destroy the monitor
    pthread_mutex_destroy(&m.mutex);
    pthread_cond_destroy(&m.cond);
    
    return 0;
}