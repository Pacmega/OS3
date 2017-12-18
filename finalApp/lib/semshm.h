#ifndef SEMSHM
#define SEMSHM

#include <semaphore.h>

#define writeSemName  /itemsToWrite
#define readSemName   /itemsToRead
#define sharedMemName OSUSBsharedMem

char * my_shm_create (int size, char* memoryName);
char * my_shm_open (int size, char* memoryName);
sem_t * my_sem_open (char* semaphoreName);

void shmCleanup (char* shmName);
void semCleanup (char* semaphoreName);

#endif