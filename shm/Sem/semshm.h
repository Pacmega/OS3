#ifndef SEMSHM
#define SEMSHM

#include <semaphore.h>

char * my_shm_create (int size);
char * my_shm_open ();
void my_sem_open (sem_t ** semaphore);

#endif