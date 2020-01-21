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
int filled_id_a;
int empty_id_a;
int filled_id_b;
int empty_id_b;

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


void producerA(void) {
    int num;
    for (;;) {
        bin_sem_wait(empty_id_a);
        bin_sem_wait(only_one_id);
        if (even < 10) {
            num = generateA();
            printf("Generated even number: %d\n", num);
            insertQ(queue, num);
            ++even;
            printQ(queue);
            sleepRandTime();
        }
        bin_sem_post(only_one_id);
        bin_sem_post(filled_id_a);
        
    }
}


void producerB(void) {
    int num;
    for (;;) {
        bin_sem_wait(empty_id_b);
        bin_sem_wait(only_one_id);
        if (even > odd) {
            num = generateB();
            printf("Generated odd number: %d\n", num);
            insertQ(queue, num);
            ++odd;
            printQ(queue);
            sleepRandTime();
        }
        bin_sem_post(only_one_id);
        bin_sem_post(filled_id_b);
    }
}

void consumerA(void) {
    int num;
    for(;;) {
        bin_sem_wait(filled_id_a);
        bin_sem_wait(only_one_id);
        numbers = queue[0];

        if (numbers >= 3 && checkHeadParityQ(queue) == 0) {
            num = popQ(queue);
            printf("Popped even number: %d\n", num);
            printQ(queue);
            --even;
            sleepRandTime();
        }
        bin_sem_post(only_one_id);
        bin_sem_post(empty_id_a);
    }
}


void consumerB(void) {
    int num;
    for(;;) {
        bin_sem_wait(filled_id_b);
        bin_sem_wait(only_one_id);
        numbers = queue[0];
        if (numbers >= 7 && checkHeadParityQ(queue) == 1) {
            num = popQ(queue);
            --odd;
            printf("Popped odd number: %d\n", num);
            printQ(queue);
            sleepRandTime();
        }
        bin_sem_post(only_one_id);
        bin_sem_post(empty_id_b);
    }
}

int main(int argc, char ** argv) {


    // allocation of the semaphores
    only_one_id = bin_sem_alloc(ONLY_ONE_KEY, IPC_CREAT);
    filled_id_a = sem_alloc(FILLED_A_KEY, IPC_CREAT);
    empty_id_a = sem_alloc(EMPTY_A_KEY, IPC_CREAT);
    filled_id_b = sem_alloc(FILLED_B_KEY, IPC_CREAT);
    empty_id_b = sem_alloc(EMPTY_B_KEY, IPC_CREAT);


    // initialization
    bin_sem_init(only_one_id, 1);
    sem_init(filled_id_a, 0);
    sem_init(filled_id_b, 0);

    if (BUFF_SIZE % 2 == 0) {
        sem_init(empty_id_a, BUFF_SIZE / 2);
    } else {
        sem_init(empty_id_a, BUFF_SIZE / 2 + 1);
    }
    sem_init(empty_id_b, BUFF_SIZE / 2);
    
    int queue_id;
    struct shmid_ds shmbuffer;
    int seg_size, n, data, sem_data;
    // allocation for shared memory
    n = BUFF_SIZE;
    
    seg_size = (n + 1) * sizeof(int);
    printf("Bytes to allocate: %d\n", seg_size);

    srand(time(NULL));
    // connection with our queue
    key_t key = ftok("/tmp", BUFF_SIZE);

    if (key < 0) {
        perror("ftok\n");
    } else {
        printf("ftok success. Key : %d\n", key);
    }

    queue_id = shmget(key, seg_size, IPC_CREAT | 0777);
    printf("Queue id: %d\n", queue_id);

    queue = (int*) shmat(queue_id, (const void *) 0, 0);
    if ((int) queue == -1) {
        perror("Shmat error\n");
        exit(1);
    }
    printf("Seg size is : %d\n", seg_size);

    printf("Actually allocated: %d\n", (int) sizeof(&queue));
    queue[0] = 0;
    initQ(queue);
    printQ(queue);
    pthread_t producer_a_thr, producer_b_thr, consumer_a_thr, consumer_b_thr, producer_c_thr, producer_d_thr;
    
    pthread_create(&producer_c_thr, NULL, producerA, (void*) NULL);
    pthread_create(&producer_d_thr, NULL, producerB, (void*) NULL);
    pthread_create(&consumer_a_thr, NULL, consumerA, (void*) NULL);
    pthread_create(&consumer_b_thr, NULL, consumerB, (void*) NULL);
    pthread_create(&producer_a_thr, NULL, producerA, (void*) NULL);
    pthread_create(&producer_b_thr, NULL, producerB, (void*) NULL);
    // pthread_create(&consumer_c_thr, NULL, consumerA, (void*) NULL);
    pthread_exit(NULL);

    
    return 0;
}