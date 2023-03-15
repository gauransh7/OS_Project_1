#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define NUM_OF_READERS_AND_WRITERS 20

sem_t sem_read;
sem_t sem_write;
sem_t sem_entry;
time_t t;
int num_read=0, num_write=0,counter=0;

void* reading(void *arg) {
    int id = *(int *)arg;
    srand((unsigned) time(&t));
    while (1) 
    {
        sem_wait(&sem_entry);
        sem_wait(&sem_read);
        counter++;
        if (counter==1) 
            sem_wait(&sem_write);
        sem_post(&sem_read);
        sem_post(&sem_entry);
        printf("Reader %d is reading\n", id);
        sleep(rand()%10); 
        printf("Reader %d has finished Reading \n", id);
        sem_wait(&sem_read);
        counter--;
        sem_post(&sem_read);
        if(counter==0)
            sem_post(&sem_write);

    } 
    pthread_exit(NULL);
}
void* writing(void *arg) {
    int id = *(int *)arg;
    srand((unsigned) time(&t));
    while(1)
    {
        sem_wait(&sem_entry);
        sem_wait(&sem_write);
        sem_post(&sem_entry);
        printf("Writer %d is writing\n", id);
        sleep(rand()%10);
        printf("Writer %d has finished Writing \n", id);
        sem_post(&sem_write);
    }
    pthread_exit(NULL);
}
int main() {
    pthread_t read_write[NUM_OF_READERS_AND_WRITERS];
    int i, id[NUM_OF_READERS_AND_WRITERS];
    srand((unsigned) time(&t));
    sem_init(&sem_read, 0, 1);
    sem_init(&sem_write, 0, 1);
    sem_init(&sem_entry, 0, 1);

    for (i = 0; i < NUM_OF_READERS_AND_WRITERS; i++) 
    {
        if(rand()%2==0)
        {
            id[i]=num_read++;
            pthread_create(&read_write[i], NULL, reading, (void *)&id[i]);
        }
        else
        {
            id[i]=num_write++;
            pthread_create(&read_write[i], NULL, writing, (void *)&id[i]);
        }
        sleep(1);
    }

    for (i = 0; i < NUM_OF_READERS_AND_WRITERS; i++) {
        pthread_join(read_write[i], NULL);
    }

    sem_destroy(&sem_read);
    sem_destroy(&sem_write);
    sem_destroy(&sem_entry);
    return 0;
}
