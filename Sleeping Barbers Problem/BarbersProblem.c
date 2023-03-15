#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define NUM_BARBERS 3
#define NUM_CHAIRS 5
#define NUM_CUSTOMERS 25

pthread_mutex_t mutex_queue = PTHREAD_MUTEX_INITIALIZER;
sem_t sem_barbers;
sem_t sem_customers;
int queue[NUM_CHAIRS];
int front = 0, rear = -1, num_waiting = 0;

void *barber(void *arg);
void *customer(void *arg);

int main() {
    pthread_t barber_threads[NUM_BARBERS];
    pthread_t customer_threads[NUM_CUSTOMERS];
    int i, id[NUM_CUSTOMERS];

    sem_init(&sem_barbers, 0, 0);
    sem_init(&sem_customers, 0, 0);

    for (i = 0; i < NUM_BARBERS; i++) {
        pthread_create(&barber_threads[i], NULL, barber, (void *)&i);
    }

    for (i = 0; i < NUM_CUSTOMERS; i++) {
        id[i] = i;
        pthread_create(&customer_threads[i], NULL, customer, (void *)&id[i]);
        sleep(1.5);
    }

    for (i = 0; i < NUM_BARBERS; i++) {
        pthread_join(barber_threads[i], NULL);
    }

    for (i = 0; i < NUM_CUSTOMERS; i++) {
        pthread_join(customer_threads[i], NULL);
    }

    sem_destroy(&sem_barbers);
    sem_destroy(&sem_customers);
    pthread_mutex_destroy(&mutex_queue);

    return 0;
}

void *barber(void *arg) {
    int id = *(int *)arg;

    while (1) {
        sem_wait(&sem_barbers); // wait for customers
        pthread_mutex_lock(&mutex_queue);
        if (num_waiting > 0) {
            int customer_id = queue[front];
            front = (front + 1) % NUM_CHAIRS;
            num_waiting--;
            pthread_mutex_unlock(&mutex_queue);
            sem_post(&sem_customers); // wake up customer
            printf("Barber %d is cutting hair of customer %d\n", id, customer_id);
            sleep(rand()%10); // cutting hair
            printf("Barber %d finished cutting hair of customer %d\n", id, customer_id);
        } else {
            pthread_mutex_unlock(&mutex_queue);
            printf("Barber %d is sleeping\n", id);
        }
    }

    pthread_exit(NULL);
}

void *customer(void *arg) {
    int id = *(int *)arg;

    pthread_mutex_lock(&mutex_queue);
    if (num_waiting < NUM_CHAIRS) {
        rear = (rear + 1) % NUM_CHAIRS;
        queue[rear] = id;
        num_waiting++;
        pthread_mutex_unlock(&mutex_queue);
        sem_post(&sem_barbers); // wake up barber
        printf("Customer %d is waiting for a haircut\n", id);
        sem_wait(&sem_customers); // wait for barber
        sleep(rand()%10); // getting haircut
    } else {
        pthread_mutex_unlock(&mutex_queue);
        printf("Customer %d left because no chairs available\n", id);
    }

    pthread_exit(NULL);
}
