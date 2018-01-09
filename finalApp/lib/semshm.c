#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/fcntl.h>
#include <semaphore.h>

#include "structs.h"
#include "semshm.h"

char * my_shm_create (int size, char* shmName)
{
    int     rtnval;
    char *  shm_addr;
    int     shm_filedescriptor = -1;
    
    shm_filedescriptor = shm_open (shmName, O_CREAT | O_EXCL | O_RDWR, 0600);
    if (shm_filedescriptor == -1)
    {
        perror ("ERROR: unable to create shared memory");
        return MAP_FAILED;
    }

    rtnval = ftruncate (shm_filedescriptor, size);
    if (rtnval != 0)
    {
        perror ("ERROR: ftruncate() failed");
        return MAP_FAILED;
    }

    shm_addr = (char *) mmap (NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, shm_filedescriptor, 0);
    if (shm_addr == MAP_FAILED)
    {
        perror ("ERROR: mmap() failed");
    }

    return (shm_addr);
}

char * my_shm_open (int size, char* shmName)
{
    //int     size;
    char *  shm_addr;
    int     shm_filedescriptor = -1;
    
    shm_filedescriptor = shm_open (shmName, O_RDWR, 0600);
    if (shm_filedescriptor == -1)
    {
        // Unable to open. Try to create it instead!
        printf("Unable to open shared memory. Attempting to create it now.\n");
        shm_addr = my_shm_create(size, shmName);
        shm_filedescriptor = shm_open (shmName, O_RDWR, 0600);
        if (shm_filedescriptor == -1)
        {
            perror ("ERROR: unable to open or create shared memory");
            return shm_addr;
        }
        else
        {
            printf("New shared memory created.\n");
        }
    }

    size = lseek (shm_filedescriptor, 0, SEEK_END);

    shm_addr = (char *) mmap (NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, shm_filedescriptor, 0);
    if (shm_addr == MAP_FAILED)
    {
        perror ("ERROR: mmap() failed");
    }

    return (shm_addr);
}

sem_t * my_sem_open (char* semaphoreName)
{
    sem_t * semaphore;

    semaphore = sem_open(semaphoreName, O_CREAT | O_EXCL, 0600, 1);

    if (semaphore == SEM_FAILED)
    {
        // Semaphore exists, try to open it
        semaphore = sem_open(semaphoreName, 0);

        if (semaphore == SEM_FAILED)
        {
            // Something else is wrong
            perror("ERROR: semaphore already exists but can't be opened");
            return NULL;
        }
        // Else, opening the existing semaphore succeeded
        return semaphore;
    }
        // Creating the semaphore succeeded
    return semaphore;
}

void shmCleanup (char* shmName)
{
    int rtnval;

    rtnval = shm_unlink (shmName);
    if (rtnval != 0)
    {
        perror ("ERROR: shm_unlink() failed");
    }
}

void semCleanup (char* semaphoreName)
{
    int rtnval;

    rtnval = sem_unlink (semaphoreName);
    if (rtnval != 0)
    {
        perror ("ERROR: sem_unlink() failed");
    }
}
