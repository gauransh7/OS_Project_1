#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

time_t t;
sem_t sem_mutex;
sem_t sem_p[5];
int state[5]={};   // here 0 represents thinking, 1 represents hungry and 2 represents eating


void* take_fork(int id)
{
    srand((unsigned) time(&t));
    sem_wait(&sem_mutex);
    state[id]=1;
    printf("Philosopher %d is hungry\n",id+ 1);
    sem_post(&sem_mutex);
    while(!(state[id]==1 && state[(id+ 4)%5]!=2 && state[(id+ 1)%5]!=2)) 
        sem_wait(&sem_p[id]);
    state[id] = 2;
    printf("Philosopher %d is Eating\n", id+ 1);
    sleep(rand()%10);
}
void* put_fork(int id)
{
    sem_wait(&sem_mutex);
    state[id] = 0;
    printf("Philosopher %d puts forks down\n",id+ 1);
    printf("Philosopher %d is thinking\n", id + 1);
    sem_post(&sem_p[(id+ 4)%5]);
    sem_post(&sem_p[(id+ 1)%5]);
    sem_post(&sem_mutex);
}
void* philosopher(void *arg)
{
    int id=*(int *)arg;
    while(1)
    {
        sleep(1);
        take_fork(id);
        put_fork(id);
    }
    pthread_exit(NULL);
}
int main() {
    pthread_t philosophers[5];
    int i, id[5];
    sem_init(&sem_mutex, 0, 1);
    for(i=0;i<5;i++)
        sem_init(&sem_p[i], 0, 0);
    for (i = 0; i < 5; i++) 
    {
        id[i]=i;
        pthread_create(&philosophers[i], NULL, philosopher, (void *)&id[i]);
        printf("Philosopher %d is thinking\n", id[i] + 1);
    }
    for (i = 0; i < 5; i++)
        pthread_join(philosophers[i], NULL);
    sem_destroy(&sem_mutex);
    for(i=0;i<5;i++)
        sem_destroy(&sem_p[i]);
    return 0;
}
