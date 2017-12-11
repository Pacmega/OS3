#ifndef SEMSHM
#define SEMSHM

char * my_shm_create (int size, char* memoryName);
char * my_shm_open (char* memoryName);
void my_sem_open (sem_t ** semaphore, char* semaphoreName);

void shmCleanup (char* shmName);
void semCleanup (char* semaphoreName);

#endif