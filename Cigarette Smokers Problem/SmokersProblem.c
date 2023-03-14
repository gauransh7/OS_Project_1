#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define NUM_SMOKERS 3

pthread_mutex_t mutex;
pthread_cond_t ingredient_available;
int ingredients[NUM_SMOKERS] = {0};
int missing_ingredient = -1;

void* agent(void* arg) {
    while (1) {
        
        // choose two ingredients to place on the table
        int ingredient1 = rand() % NUM_SMOKERS;
        int ingredient2;
        do {
            ingredient2 = rand() % NUM_SMOKERS;
        } while (ingredient2 == ingredient1);

        pthread_mutex_lock(&mutex);     

        // Place the ingredients on the table
        ingredients[ingredient1] = 1;
        ingredients[ingredient2] = 1;

        int lower_index = ingredient1 < ingredient2 ? ingredient1 : ingredient2;
        int higher_index = ingredient1 < ingredient2 ? ingredient2 : ingredient1;
        printf("Agent: Placed %s and %s on the table.\n", 
                lower_index == 0 ? "tobacco" : (lower_index == 1 ? "paper" : "matches"), 
                higher_index == 0 ? "tobacco" : (higher_index == 1 ? "paper" : "matches"));

        pthread_cond_broadcast(&ingredient_available);  // Signal to the condition variable that the ingredients are available

        pthread_mutex_unlock(&mutex);   
        usleep(90000);      
    }
}

void* smoker(void* arg) {
    int id = *(int*)arg;

    while (1) 
    {
        pthread_mutex_lock(&mutex);    

        // Check which are the missing ingredients
        int missing1, missing2;
        if (id == 0) {
            missing1 = 1;
            missing2 = 2;
        } else if (id == 1) {
            missing1 = 0;
            missing2 = 2;
        } else {
            missing1 = 0;
            missing2 = 1;
        }
        if (!ingredients[missing1] || !ingredients[missing2]) 
        {
            // Wait for the necessary ingredient(s) to be placed on the table
            printf("Smoker %d: Waiting for %s and %s...\n", 
                    id, 
                    missing1 == 0 ? "tobacco" : (missing1 == 1 ? "paper" : "matches"),
                    missing2 == 0 ? "tobacco" : (missing2 == 1 ? "paper" : "matches"));
            pthread_cond_wait(&ingredient_available, &mutex);
        } 
        else 
        {
            // Take the ingredients and smoke
            printf("Smoker %d: Taking %s and %s from the table.\n", 
                    id, 
                    id == 0 ? "paper" : (id == 1 ? "matches" : "tobacco"), 
                    id == 0 ? "matches" : (id == 1 ? "tobacco" : "paper"));
            printf("Smoker %d: Smoking...\n", id);

            // Reset the ingredients on the table
            ingredients[(id+1)%NUM_SMOKERS] = 0;
            ingredients[(id+2)%NUM_SMOKERS] = 0;
        }

        pthread_mutex_unlock(&mutex);       
        usleep(90000);      
    }
}

int main() {
    srand(time(NULL));

    // Initialize the mutex and condition variable
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&ingredient_available, NULL);

    pthread_t agent_thread;
    pthread_create(&agent_thread, NULL, agent, NULL);

    pthread_t smoker_threads[NUM_SMOKERS];
    int smoker_ids[NUM_SMOKERS] = {0, 1, 2};
    for (int i = 0; i < NUM_SMOKERS; i++) {
        pthread_create(&smoker_threads[i], NULL, smoker, &smoker_ids[i]);
    }

    // Wait for all threads to finish
    pthread_join(agent_thread, NULL);
    for (int i = 0; i < NUM_SMOKERS; i++) {
        pthread_join(smoker_threads[i], NULL);
    }

    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&ingredient_available);

    return 0;
}