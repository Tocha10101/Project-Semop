#include <stdlib.h>
#include <unistd.h>

// queue is simply an array
// queue[0] is the length of this array


// no need to check if the queue is full
// in other place this was checked

void insertQ(int * queue, int el) {
    queue[queue[0] + 1] = el;
    ++queue[0];
}


int popQ(int * queue) {
    int el_to_pop = queue[1];
    int i;
    for (i = 1; i < queue[0]; i++) {
        queue[i] = queue[i + 1];
    }
    --queue[0];
    return el_to_pop;
}

int getSize(int * queue) {
    return queue[0];
}

void printQ(int * queue) {
    printf("Lenght: %d\nElements:\t", getSize(queue));
    int i;
    for (i = 0; i < getSize(queue); i++) {
        printf("%d\t", queue[i + 1]);
    }
    printf("\n");
}

int getEvenQ(int * queue) {
    int even_num = 0;
    for (int i = 1; i < queue[0]; i++) {
        if (queue[i] % 2 == 0) {
            ++even_num;
        }
    }
    return even_num;
}

int getOddQ(int * queue) {
    int odd_num = 0;
    for (int i = 0; i < queue[0]; i++) {
        if (queue[i] % 2) {
            ++odd_num;
        }
    }
    return odd_num;
}

int checkHeadParityQ(int * queue) {
    return queue[1] % 2;
}

void sleepRandTime() {
    int quantum = 1 + rand() % 5;
    sleep(quantum);
}