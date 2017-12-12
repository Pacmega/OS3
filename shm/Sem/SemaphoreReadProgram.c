#include <stdio.h>      // Printing to the terminal
#include <string.h>     // Strings
#include <sys/types.h>  // Shared memory
#include <sys/mman.h>   // Shared memory
#include <sys/stat.h>   // Shared memory
#include <sys/fcntl.h>  // Shared memory
#include <semaphore.h>  // Semaphores
#include <signal.h>     // For the keyboard interrupt
#include <stdbool.h>    // Booleans
#include <pthread.h>    // Threads
#include <unistd.h>     // Sleep()

#include "structs.h"    // Data about the multithreading and number structs
#include "semshm.h"     // Semaphore & shared memory management functions

#define NUMBERARRAYLENGTH 9

// This needs to be global for the interrupt handler to be able to change it.
bool interruptDetected = false;

void safeExit (sem_t * semaphoreToTest);

void  InterruptHandler(int sig);
void cleanUp();

int main(int argc, char const *argv[])
{
	// Set up the interrupt for pressing ctrl-C, so it doesn't kill the program.
    // Instead, the program starts closes the semaphore & shared memory.
    signal(SIGINT, InterruptHandler);
    
    multithreading MTstruct;

    MTstruct.sharedMem = (char *) MAP_FAILED;
    MTstruct.itemsFilled = SEM_FAILED;
    MTstruct.spaceLeft = SEM_FAILED;

    char * memoryName = "OSassSharedMem";
    char * itemsFilledSemName = "itemsFilled";
    char * spaceLeftSemName = "spaceLeft";

	int rtnval;
	int positionToRead;
	number readNr;

	number* shm_number = (number*)MTstruct.sharedMem;

	// Other variables that are used throughout the program.
	int         numberStructSize = sizeof(number);
	int         numberArraySize = NUMBERARRAYLENGTH * numberStructSize;

	// We don't need to check if any of these already exist, that is not actually possible.
	// When the program is first run, they logically do not exist yet so we create them.
	// When the program ends, it also removes the shared memory and semaphores from the disk. This means they need to be created again.
	MTstruct.sharedMem = my_shm_create(numberArraySize, memoryName);  // Create the shared memory and save the address
	my_sem_open(&MTstruct.itemsFilled, itemsFilledSemName);           // Create the semaphore to count how many items are available
	my_sem_open(&MTstruct.spaceLeft, spaceLeftSemName);               // Create the semaphore to count how much space is left to write to

	if (MTstruct.itemsFilled == SEM_FAILED)
	{
	    printf("Critical error: unable to open itemsFilled semaphore.");

	    // Unable to properly execute without semaphore, shut down.
	    return -1;
	}
	if (MTstruct.spaceLeft == SEM_FAILED)
	{
	    printf("Critical error: unable to open spaceLeft semaphore.");

	    // Unable to properly execute without semaphore, shut down.
	    return -1;
	}
	if (MTstruct.sharedMem == MAP_FAILED)
	{
	    printf("Critical error: unable to open or create shared memory.\n");

	    // Unable to properly execute without shared memory, shut down.
	    return -1;
	}

	while(!interruptDetected)
	{
	    rtnval = sem_wait(MTstruct.itemsFilled);
	    if(rtnval != 0)
	    {
	        perror("ERROR: sem_wait() failed");
	        break;
	    }

	    // Get the position where there is space to write.
	    rtnval = sem_getvalue(MTstruct.itemsFilled, &positionToRead);
	    if(rtnval != 0)
	    {
	        perror("ERROR: sem_getvalue() failed");
	        break;
	    }

	    readNr = shm_number[positionToRead];
	    
	    rtnval = sem_post(MTstruct.spaceLeft);
	    if(rtnval != 0)
	    {
	        perror("ERROR: sem_post() failed");
	        break;
	    }

	    printf("%d - %s\n", readNr.value, readNr.pronunciation);
	}

	// Check if the spaceLeft semaphore is 0 and if so post to it, to avoid the other thread getting a deadlock.
	safeExit(MTstruct.spaceLeft);

	printf("Reading thread reached end.\n");

	return 0;
}

// Check if the passed semaphore is 0 and if so post to it, to avoid deadlocks when one of the threads exits first.
void safeExit (sem_t * semaphoreToTest)
{
    int semaphoreValue;
    int rtnval;

    rtnval = sem_getvalue(semaphoreToTest, &semaphoreValue);
    if(rtnval != 0)
    {
        perror("ERROR: sem_getvalue() failed");
    }

    if (semaphoreValue == 0)
    {
        rtnval = sem_post(semaphoreToTest);
        if(rtnval != 0)
        {
            perror("ERROR: sem_post() failed");
        }
    }
}

void  InterruptHandler(int sig)
{
    signal(sig, SIG_IGN);
    printf("Keyboard interrupt detected. Shutting down...\n");
    interruptDetected = true;
}
