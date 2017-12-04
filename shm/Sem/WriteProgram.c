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

number createNr(int value, char* pronunciation)
{
    number newNr;
    newNr.value = value;
    newNr.pronunciation = pronunciation;

    return newNr;
}

void createStructs(number numberArray[])
{
    numberArray[0] = createNr(1, "Ace");
    numberArray[1] = createNr(2, "Deuce");
    numberArray[2] = createNr(3, "Trey");
    numberArray[3] = createNr(4, "Cater");
    numberArray[4] = createNr(5, "Cinque");
    numberArray[5] = createNr(6, "Sice");
    numberArray[6] = createNr(7, "Seven");
    numberArray[7] = createNr(8, "Eight");
    numberArray[8] = createNr(9, "Nine");
}

int main(void)
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

    // When this point in the program is reached, the semaphore and shared memory have been
    // prepared for usage. Now, create the array of numbers.

    number numberArray[9];
    createStructs(numberArray);
    number* shm_number = (number*)shm_addr;

    while(1)
    {
    	// This continues forever.
    	int i = 0;
    	for(; i < NUMBERARRAYLENGTH; i++)
    	{
    	    // Write the numbers in the struct to the shared memory one by one.
    	    *shm_number = numberArray[i];
    	    rtnval = sem_post(sem_addr);
    	    if(rtnval != 0)
    	    {
    	        perror("ERROR: sem_post() failed");
    	        break;
    	    }
    	}
    }

    // Program should not be able to reach end in normal flow, so if end is reached return an error code.
    return -1;
}
