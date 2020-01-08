#include "sem.h"
#include <sys/ipc.h>
#include <sys/sem.h>

// Allocates the binary semaphore in neccesary.
// Obtains semaphore ID
int bin_sem_alloc(key_t key, int sem_flags) {
    return semget(key, 1, sem_flags);
}

// Deallocates the binary semaphore. Gets ID
int bin_sem_dealloc(int semid) {
    union semun arg;    // ignored
    return semctl(semid, 1, IPC_RMID, arg);
}

// initializes a binary semaphore
// with a value 1
int bin_sem_init(int semid, int value) {
    union semun arg;
    unsigned short values[1];
    values[0] = value;
    arg.array = values;
    return semctl(semid, 0, SETALL, arg);
}

// wait (P(s)) operation
int bin_sem_wait(int semid) {
    struct sembuf operations[1];
    operations[0].sem_num = 0;
    // decrement by 1
    operations[0].sem_op = -1;
    // allow to undo
    operations[0].sem_flg = SEM_UNDO;
    return semop(semid, operations, 1);
}

// post (V(s)) operation
int bin_sem_post(int semid) {
    struct sembuf operations[1];
    operations[0].sem_num = 0;
    // increment by 1
    operations[0].sem_op = 1;
    // allow to undo
    operations[0].sem_flg = SEM_UNDO;
    return semop(semid, operations, 1);
}

// operations for non-binary semaphores
// DO IT!!!

int sem_alloc(key_t key, int sem_flags) {
    return semget(key, 1, sem_flags);
}

int sem_dealloc(int semid) {
    union semun arg;
    return semctl(semid, 1, IPC_RMID, arg);
}

int sem_init(int semid, int value) {
    union semun arg;
    unsigned short values[1];
    values[0] = value;
    arg.array = values;
    return semctl(semid, 0, SETALL, arg);
}

int sem_get_val(int semid) {
    return semctl(semid, 0, GETVAL, 0);
}

int sem_check(int semid) {
    struct sembuf sem = {0, -1, IPC_NOWAIT | SEM_UNDO};
    if (semop(semid, &sem, 1) < 0) {
        return 0;
    }
    return 1;
}

int sem_change(int semid, int value) {
    if (value == 0)
        return -1;
    if (value < 0)
        value = -1;
    else
        value = 1;
    struct sembuf sem = {0, value, SEM_UNDO};
    return semop(semid, &sem, 1);
}