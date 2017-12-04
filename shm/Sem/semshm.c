#include "semshm.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/fcntl.h>
#include <semaphore.h>

#include "number.h"

char * my_shm_create (int size)
{
    char* shm_fixedName = "memory";

    int     rtnval;
    char *  shm_addr;
    
    // printf ("Calling shm_open('%s')\n", shm_fixedName);
    shm_fd = shm_open (shm_fixedName, O_CREAT | O_EXCL | O_RDWR, 0600);
    if (shm_fd == -1)
    {
        perror ("ERROR: shm_open() failed");
    }
    // printf ("shm_open() returned %d\n", shm_fd);
                
    // printf ("Calling ftrucate(%d,%d)\n", shm_fd, size);
    rtnval = ftruncate (shm_fd, size);
    if (rtnval != 0)
    {
        perror ("ERROR: ftruncate() failed");
    }
    // printf ("ftruncate() returned %d\n", rtnval);
                
    // printf ("Calling mmap(len=%d,fd=%d)\n", size, shm_fd);
    shm_addr = (char *) mmap (NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (shm_addr == MAP_FAILED)
    {
        perror ("ERROR: mmap() failed");
    }
    printf ("mmap() returned %p\n", shm_addr);

    return (shm_addr);
}

char * my_shm_open ()
{
    char* shm_fixedName = "memory";

    int     size;
    char *  shm_addr;
    
    // printf ("Calling shm_open('%s')\n", shm_name);
    shm_fd = shm_open (shm_name, O_RDWR, 0600);
    if (shm_fd == -1)
    {
        perror ("ERROR: shm_open() failed");
    }
    // printf ("shm_open() returned %d\n", shm_fd);
                
    // printf ("Calling lseek(fd=%d,SEEK_END)\n", shm_fd);
    size = lseek (shm_fd, 0, SEEK_END);
    // printf ("lseek() returned %d\n", size);
                
    // printf ("Calling mmap(len=%d,fd=%d)\n", size, shm_fd);
    shm_addr = (char *) mmap (NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (shm_addr == MAP_FAILED)
    {
        perror ("ERROR: mmap() failed");
    }
    printf ("mmap() returned %p\n", shm_addr);

    return (shm_addr);
}

void my_sem_open (sem_t ** semaphore)
{
    char* sem_fixedName = "semaphore";

    if (*semaphore != SEM_FAILED)
    {
        printf ("ERROR: another semaphore already opened\n");
        return;
    }

    // printf("Calling sem_open on semaphore with name %s\n", sem_fixedName);
    *semaphore = sem_open(sem_fixedName, O_CREAT | O_EXCL, 0600, 1);

    if (*semaphore == SEM_FAILED)
    {
        // File exists, try to open it
        *semaphore = sem_open(sem_fixedName, 0);

        if (*semaphore == SEM_FAILED)
        {
            // Something else is wrong
            perror("ERROR: semaphore already exists");
        }
    }

    printf("sem_open() returned %p\n", *semaphore);
}