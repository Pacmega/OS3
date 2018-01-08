#ifndef SEMSHM
#define SEMSHM

#include <semaphore.h>

<<<<<<< HEAD
=======
#define writeSemName  /itemsToWrite
#define readSemName   /itemsToRead
#define sharedMemName OSUSBsharedMem

>>>>>>> a7d839b995e1014a6dfb4bf955cddb0a8660a2ba
char * my_shm_create (int size, char* memoryName);
char * my_shm_open (int size, char* memoryName);
sem_t * my_sem_open (char* semaphoreName);

void shmCleanup (char* shmName);
void semCleanup (char* semaphoreName);

#endif