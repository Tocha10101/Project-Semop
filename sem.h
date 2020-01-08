#include <sys/types.h>
#include <errno.h>


union semun {
    int val;
    struct semid_ds *buf;       // buffer for waiting processes
    unsigned short int *array;  // the array of semaphore possible 
                                // values
    // struct seminfo *__buf;
};

// Allocates the binary semaphore in neccesary.
// Obtains semaphore ID
int bin_sem_alloc(key_t, int);

// Deallocates the binary semaphore. Gets ID
int bin_sem_dealloc(int);

// initializes a binary semaphore
int bin_sem_init(int, int);

// wait (P(s)) operation
int bin_sem_wait(int);

// post (V(s)) operation
int bin_sem_post(int);


// all the operations for non-binary semaphores
// DO IT!!!

int sem_change(int,int);
int sem_alloc(key_t key, int sem_flags);
int sem_dealloc(int semid);
int sem_init(int semid, int value);
int sem_get_val(int semid);
