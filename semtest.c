#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <sys/types.h>
#include <time.h>
#include <sys/shm.h>
#include <unistd.h>
#include "keys.h"
#include "sem.c"
#include "queue.c"

int only_one_id;
int mutex_num_id;
int mutex_even_id;
int mutex_odd_id;
int filled_id;
int empty_id;

int * queue;    // our FIFO buffer

int even = 0;
int odd = 0;
int numbers = 0;

// nieparzyste
int generateB() {
    int value = rand() % 100;
    return value % 2 ? value : value + 1;
}

// parzyste
int generateA() {
    int value = rand() % 100;
    return value % 2 ? value + 1 : value;
}


void producerA() {
    int num;
    for (;;) {
        bin_sem_wait(only_one_id);
        // bin_sem_wait(empty_id);
        sem_change(empty_id, -1);           // down on non-binary semaphore
        bin_sem_wait(mutex_num_id);
        bin_sem_wait(mutex_even_id);
        if (even < 10) {
            num = generateA();
            printf("Generated even number: %d\n", num);
            insertQ(queue, num);
            printQ(queue);
        }
        ++numbers;
        ++even;
        bin_sem_post(mutex_even_id);
        bin_sem_post(mutex_num_id);
        // bin_sem_post(filled_id);
        sem_change(filled_id, 1);              // up on non-binary semaphore
        bin_sem_post(only_one_id);
        sleepRandTime();
    }
}


void producerB() {
    int num;
    for (;;) {
        bin_sem_wait(only_one_id);
        // bin_sem_wait(empty_id);
        sem_change(empty_id, -1);
        bin_sem_wait(mutex_num_id);
        bin_sem_wait(mutex_even_id);
        bin_sem_wait(mutex_odd_id);

        if (even > odd) {
            num = generateB();
            printf("Generated odd number: %d\n", num);
            insertQ(queue, num);
            printQ(queue);
        }
        ++odd;
        ++numbers;
        bin_sem_post(mutex_odd_id);
        bin_sem_post(mutex_even_id);
        bin_sem_post(mutex_num_id);
        // bim_sem_post(filled_id);
        sem_change(filled_id, 1);
        bin_sem_post(only_one_id);
        sleepRandTime();
    }
}

void consumerA() {
    int num;
    for(;;) {
        bin_sem_wait(only_one_id);
        // bin_sem_wait(filled_id);
        sem_change(filled_id, -1);
        bin_sem_wait(mutex_num_id);
        
        if (numbers >= 3 && checkHeadParityQ(queue) == 0) {
            num = popQ(queue);
            printf("Popped even number: %d", num);
            printQ(queue);
        }
        --numbers;
        bin_sem_post(mutex_num_id);
        // bin_sem_post(empty_id);
        sem_change(empty_id, 1);
        bin_sem_post(only_one_id);
        sleepRandTime();
    }
}


void consumerB() {
    int num;
    for(;;) {
        bin_sem_wait(only_one_id);
        sem_change(filled_id, -1);
        bin_sem_wait(mutex_num_id);
        
        if (numbers >= 7 && checkHeadParityQ(queue) == 1) {
            num = popQ(queue);
            printf("Popped odd number: %d\n", num);
            printQ(queue);
        }
        --numbers;
        bin_sem_post(mutex_num_id);
        sem_change(empty_id, 1);
        bin_sem_post(only_one_id);
        sleepRandTime();
    }
}

int main(int argc, char ** argv) {


    // allocation of the semaphores
    only_one_id = bin_sem_alloc(ONLY_ONE_KEY, IPC_CREAT);
    mutex_num_id = bin_sem_alloc(NUM_KEY, IPC_CREAT);
    mutex_even_id = bin_sem_alloc(EVEN_KEY, IPC_CREAT);
    mutex_odd_id = bin_sem_alloc(ODD_KEY, IPC_CREAT);
    filled_id = sem_alloc(FILLED_KEY, IPC_CREAT);
    empty_id = sem_alloc(EMPTY_KEY, IPC_CREAT);


    // initialization
    bin_sem_init(only_one_id, 1);
    bin_sem_init(mutex_num_id, 1);
    bin_sem_init(mutex_even_id, 1);
    bin_sem_init(mutex_odd_id, 1);
    sem_init(filled_id, 0);
    sem_init(empty_id, BUFF_SIZE);

    
    int queue_id;
    struct shmid_ds shmbuffer;
    int seg_size, n, data, sem_data;
    // allocation for shared memory
    if (argc == 2) {
        n = atoi(argv[1]);
        seg_size = (n + 1) * sizeof(int);
        printf("Bytes to allocate: %d\n", seg_size);
    }
    srand(time(NULL));
    // connection with our queue
    key_t key = ftok("shmbuff", 10); // this is misterious
    queue_id = shmget(key, seg_size, 0666 | IPC_CREAT);
    printf("Queue id: %d\n", queue_id);
    queue = (int*) shmat(queue_id, (void *)0, 0);
    printf("Actually allocated: %d\n", sizeof(queue));
    printf("Second phase\n");
    queue[0] = 0;               // here is a segmentation fault
    printf("After the queue things\n");
    //pthread_t producer_a_thr, producer_b_thr, consumer_a_thr, consumer_b_thr;
    pthread_t thread_id;
    printf("Before threads\n");
    pthread_create(&thread_id, NULL, producerA, (void *) &thread_id);
    pthread_create(&thread_id, NULL, producerB, (void *) &thread_id);
    pthread_create(&thread_id, NULL, consumerA, (void *) &thread_id);
    pthread_create(&thread_id, NULL, consumerB, (void *) &thread_id);

    // pthread_join(&producer_a_thr, NULL);
    // pthread_join(&producer_b_thr, NULL);
    // pthread_join(&consumer_a_thr, NULL);
    // pthread_join(&consumer_b_thr, NULL);
    pthread_exit(NULL);

    printf("After threads\n");

    // int alpha;
    // int betha;
    // for (int i = 0; i < 10; i++) {
    //     alpha = generateB();
    //     printf("Generated number: %d\n", alpha);
    // }
    
    // printf("Something\n");
    return 0;
}