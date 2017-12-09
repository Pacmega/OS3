#include <stdio.h>		// 
#include <string.h>		// 
#include <unistd.h>		// 
#include <errno.h>		// 
#include <sys/types.h>	// 
#include <sys/mman.h>	// 
#include <sys/stat.h>	// 
#include <sys/fcntl.h>	// 
#include <semaphore.h>	// Semaphores
#include <signal.h>		// For the keyboard interrupt
#include <stdbool.h>	// Booleans
#include <pthread.h>	// Threads

#include "structs.h" 	// Data about the multithreading and number structs
#include "semshm.h" 	// Semaphore & shared memory management functions

#define NUMBERARRAYLENGTH 9

// It seems like this one needs to be global for the interrupt handler to be able to change it.
bool interruptDetected = false;

static void * writingThread (void * arg);
static void * readingThread (void * arg);

number createNr(int value, char* pronunciation);
void createStructs(number numberArray[]);
void  InterruptHandler(int sig);
void cleanUp();

int main(void)
{
	// Set up the interrupt for pressing Ctrl-C, so it doesn't kill the program.
    // Instead, the program starts closes the semaphore & shared memory.
    signal(SIGINT, InterruptHandler);

	// Separate threads for reading and writing
	pthread_t   writeThread;
    pthread_t   readThread;

    // Semaphore and Shared Memory
    multithreading MTstruct;
    MTstruct.sharedMem = (char *) MAP_FAILED;
    MTstruct.semaphore = SEM_FAILED;

    // Other variables that are used throughout the program.
    int         numberStructSize = sizeof(number);
    int         numberArraySize = NUMBERARRAYLENGTH * numberStructSize;

    printf("[DBG] Annihilating existing shm & sem\n");
    shmCleanup();
    semCleanup(&MTstruct.semaphore);

    // Names for both shared memory and the semaphore are set in semshm.c
    MTstruct.sharedMem = my_shm_open();	// Open the shared memory and save the address
    my_sem_open(&MTstruct.semaphore);		// Open the passed semaphore

    if (MTstruct.semaphore == SEM_FAILED)
    {
    	printf("Critical error: unable to open semaphore.");

    	// Unable to properly execute without semaphore, shut down.
    	return -1;
    }
    else if (MTstruct.sharedMem == MAP_FAILED)
    {
    	// Attempt to create the SHM instead of opening it

    	MTstruct.sharedMem = my_shm_create(numberArraySize);

    	if (MTstruct.sharedMem == MAP_FAILED)
    	{
    		printf("Critical error: unable to open or create shared memory.\n");

    		// Unable to properly execute without shared memory, shut down.
    		return -1;
    	}
    }

    // Create both threads
    if (pthread_create (&writeThread, NULL, writingThread, &MTstruct) != 0)
    {
        perror ("writing thread");
    }
    if (pthread_create (&readThread, NULL, readingThread, &MTstruct) != 0)
    {
        perror ("reading thread");
    }

    (void) pthread_join(writeThread, NULL);
    (void) pthread_join(readThread, NULL);
    
    /* Mass commented because this was all moved to threads but isn't entirely functional yet
    while(!interruptDetected)
    {
    	// This continues until ctrl-c is pressed.
    	int i = 0;
    	for(; i < NUMBERARRAYLENGTH; i++)
    	{
    	    // Write the numbers in the struct to the shared memory one by one.
    	    *shm_number = numberArray[i];
    	    rtnval = sem_post(MTstruct.semaphore);
    	    if(rtnval != 0)
    	    {
    	        perror("ERROR: sem_post() failed");
    	        break;
    	    }

	    	rtnval = sem_wait(MTstruct.semaphore);
	        if(rtnval != 0)
	        {
	            perror("ERROR: sem_wait() failed");
	            break;
	        }

	        readNr = *shm_number;
	        // printf("%d - %s\n", readNr.value, readNr.pronunciation);
	        printf("Sem: %s\n", MTstruct.semaphore);
    	}
    }
    */

    // TODO: create multithreading to be able to handle input
    //       Note: make a sleep before commencing the mass R/W
    // TODO: using multithreading, merge read & write code
    // TODO: using multithreading, catch ctrl-C as an interrupt to shut down
    // TODO: implement a buffer to read & write from multiple positions
    // TODO: somewhere here, multiple semaphores 

    // Program should not be able to reach end in normal flow, so if end is reached return an error code.

    return -1;
}

// Code for both threads

static void * writingThread (void * arg)
{
	// Convert the given argument to a Multithreading pointer
	multithreading * MTstruct = (multithreading *) arg;

	int rtnval;
	int semValue;
	number numberArray[9];
    createStructs(numberArray);

    number* shm_number = (number*)MTstruct->sharedMem;

    while(!interruptDetected)
    {
    	// Write the numbers in the struct to the shared memory one by one.
    	rtnval = sem_getvalue (MTstruct->semaphore, &semValue);
    	if(rtnval != 0)
    	{
    		perror("ERROR: sem_getvalue() failed");
    		break;
    	}

	    *shm_number = numberArray[semValue];
	    
		rtnval = sem_post(MTstruct->semaphore);
		// printf("[DBG] writingThread while loop\n");
	    
	    if(rtnval != 0)
	    {
	        perror("ERROR: sem_post() failed");
	        break;
	    }

	    rtnval = sem_wait(MTstruct->semaphore);
		if(rtnval != 0)
        {
            perror("ERROR: sem_wait() failed");
            break;
        }
    }

	return (NULL);
}

static void * readingThread (void * arg)
{
	// Convert the given argument back to a Multithreading struct
	multithreading * MTstruct = (multithreading *) arg;

	int rtnval;
	int semValue;
	number readNr;

	number* shm_number = (number*)MTstruct->sharedMem;

	while(!interruptDetected)
    {
    	rtnval = sem_wait(MTstruct->semaphore);

    	printf("[DBG] readingThread while loop\n");
        if(rtnval != 0)
        {
            perror("ERROR: sem_wait() failed");
            break;
        }

        rtnval = sem_getvalue (MTstruct->semaphore, &semValue);
    	if(rtnval != 0)
    	{
    		perror("ERROR: sem_getvalue() failed");
    		break;
    	}

    	// TODO: how to read further down the array?
        readNr = *shm_number;
        printf("%d - %s\n", readNr.value, readNr.pronunciation);
    }

	return (NULL);
}

// All other functions

number createNr(int value, char* pronunciation)
{
	number newNr;
    newNr.value = value;
    newNr.pronunciation = pronunciation;

    return newNr;
}

void createStructs(number numberArray[])
{
	// This function only works specifically with an array of 9 or more 
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

void  InterruptHandler(int sig)
{
    signal(sig, SIG_IGN);
    printf("Keyboard interrupt detected. Shutting down...\n");
    interruptDetected = true;
}