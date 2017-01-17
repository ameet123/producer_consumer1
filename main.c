//
// Created by achaub001c on 1/15/2017.
//
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "util.h"

#define TRUE 1
#define RESET   "\033[0m"
#define BLACK   "\033[30m"      /* Black */
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define BOLDGREEN   "\033[1m\033[32m"
#define BOLDRED     "\033[1m\033[31m"

int PRODUCERS;
int CONSUMERS;
int QUEUE_SIZE; // the max queue size
int MAX_ENTRIES; // items to generate provided by user.

// buffer or queue through which producers and consumers communicate
// since we will get the number from command line, we are declaring that as a pointer to be malloc'ed.
int *buf;

int add = 0; // index to add the number at
int rem = 0; // index to remove the number at
int counter = 0; // how many items produced
int num = 0; // elements currently in buffer

// Mutex & conditions
pthread_mutex_t m = PTHREAD_MUTEX_INITIALIZER; // single mutex for both producer/consuer to lock the queue/buffer
pthread_cond_t buffer_not_empty_cond = PTHREAD_COND_INITIALIZER;  // to wake up consumer
pthread_cond_t buffer_not_full_cond = PTHREAD_COND_INITIALIZER;

/**
 * run by consumers
 * @param arg
 * @return
 */
void *consume(void *arg) {
    char curr_time[9]; // to print time
    while (TRUE) {
        if (num < 0) {
            perror("Underflow error\n");
            pthread_exit(NULL);
        }
        // lock mutex
        pthread_mutex_lock(&m);
        // try to wait on consumer condition which is that buffer needs some elements
        while (num == 0) {
            pthread_cond_wait(&buffer_not_empty_cond, &m);
        }
        // if we arrive here, that means something to consume
        int consumed = buf[rem];
        rem = (rem + 1) % QUEUE_SIZE;
        get_milli_time(curr_time);
        printf(BOLDRED"\t\t[%d]"RESET" %s-> Consumed:"RED"%d"RESET"\n", (unsigned int) pthread_self(), curr_time,
               consumed);
        fflush(stdout);
        // decrement the element in buffer counter
        num--;
        // release lock
        pthread_mutex_unlock(&m);
        pthread_cond_signal(&buffer_not_full_cond);
    }

    printf("\n");
    pthread_exit(NULL);
}

/**
 * run by producers, could be many
 * observation - the queue/buff is the main entity in 'critical section' and
 * it needs to be locked by both producer and consumers, so it could be a bottle neck
 * @param arg
 * @return
 */
void *produce(void *arg) {
    srand((unsigned int) time(NULL));
    char t_buf[9];
    while (TRUE) {
        // lock the mutex
        pthread_mutex_lock(&m);
        if (counter == MAX_ENTRIES) {
            printf("Max entries[%d] produced, exiting\n", MAX_ENTRIES);
            exit(0);
        }
        // ensure that buff is not full
        while (num == QUEUE_SIZE) {
            pthread_cond_wait(&buffer_not_full_cond, &m);
        }
        // generate random  number
        get_milli_time(t_buf);
        buf[add] = rand() % 20;
        printf(BOLDGREEN"[%d]"RESET" %s-> Produced:"GREEN" %d "RESET"\n", (unsigned int) pthread_self(), t_buf,
               buf[add]);
        fflush(stdout);
        add = (add + 1) % QUEUE_SIZE; // this is a circular buffer
        counter++;
        num++;  // added 1 element in buffer, now tell consumer
        // wake up consumer to tell them of new production
        pthread_cond_signal(&buffer_not_empty_cond);
        pthread_mutex_unlock(&m);
        usleep(40);
    }
}

/**
 * spawn threads here
 * @return
 */
int main(int argc, char *argv[]) {
    if (argc != 4) {
        printf("Usage: ./producer_consumer [# of producers] [ # of consumers] [ # max entries]");
        exit(1);
    }
    PRODUCERS = atoi(argv[1]);
    CONSUMERS = atoi(argv[2]);
    MAX_ENTRIES = atoi(argv[3]);

    // define buf the queue size
    buf = malloc(CONSUMERS * sizeof(int));
    QUEUE_SIZE = CONSUMERS;

    pthread_t consumers[CONSUMERS];
    pthread_t producers[PRODUCERS];
    pthread_t prod_t[PRODUCERS], cons_t[CONSUMERS];
    for (int i = 0; i < PRODUCERS; ++i) {
        pthread_create(&prod_t[i], NULL, produce, NULL);
    }
    for (int i = 0; i < CONSUMERS; ++i) {
        pthread_create(&cons_t[i], NULL, consume, NULL);
    }
    // wait
    for (int i = 0; i < PRODUCERS; ++i) {
        pthread_join(prod_t[i], NULL);
    }
    for (int i = 0; i < CONSUMERS; ++i) {
        pthread_join(cons_t[i], NULL);
    }
}

