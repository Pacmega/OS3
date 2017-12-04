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
#include "semshm.h"

#define NUMBERARRAYLENGTH 9

int main()
{
	char *      shm_addr = (char *) MAP_FAILED;
	sem_t *     sem_addr = SEM_FAILED;
	int         numberStructSize = sizeof(number);
	int         numberArraySize = NUMBERARRAYLENGTH * numberStructSize;
	int         rtnval;

	// Names for both shared memory and the semaphore are set in semshm.c
	shm_addr = my_shm_open();
	my_sem_open(&sem_addr);

	if (sem_addr == SEM_FAILED)
    {
    	printf("Critical error: unable to open semaphore.");

    	// Unable to properly execute without semaphore, shut down.
    	return -1;
    }
    else if (shm_addr == MAP_FAILED)
    {
    	// Attempt to create the SHM instead of opening it

    	shm_addr = my_shm_create(numberArraySize);

    	if (shm_addr == MAP_FAILED)
    	{
    		printf("Critical error: unable to open or create shared memory.\n");

    		// Unable to properly execute without shared memory, shut down.
    		return -1;
    	}
    }

    number* shm_loc = (number*)shm_addr;
    number  readNr;

    while(1)
    {
        // This continues forever.
        
        rtnval = sem_wait(sem_addr);
        if(rtnval != 0)
        {
            perror("ERROR: sem_wait() failed");
            break;
        }

        readNr = *shm_loc;
        printf("%d - %s\n", readNr.value, readNr.pronunciation);
    }

	// End of program should never be reached
	return -1;

	/*	MANY THINGS LEFT TO DO:
		- See the TODO list in WriteProgram for more details
		- Multithreading!
		- More semaphores!
	*/
}