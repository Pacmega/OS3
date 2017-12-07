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

#include "structs.h" 	// Data about the multithreading and number structs
#include "semshm.h" 	// Semaphore & shared memory management functions

#define NUMBERARRAYLENGTH 9;

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
    int         rtnval;

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

    // When this point in the program is reached, the semaphore and shared memory have been
    // prepared for usage. Now, create the array of numbers.

    number numberArray[9];
    createStructs(numberArray);
    number* shm_number = (number*)MTstruct.sharedMem;

    // TODO: Part of testing, fix location (separate thread, most likely)
    number readNr;

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
	        printf("%d - %s\n", readNr.value, readNr.pronunciation);
    	}
    }

    // TODO: create multithreading to be able to handle input
    //       Note: make a sleep before commencing the mass R/W
    // TODO: using multithreading, merge read & write code
    // TODO: using multithreading, catch ctrl-C as an interrupt to shut down
    // TODO: implement a buffer to read & write from multiple positions
    // TODO: somewhere here, multiple semaphores 

    // Program should not be able to reach end in normal flow, so if end is reached return an error code.

    cleanUp();
    return -1;
}

// Code for both threads

static void * writingThread (void * arg)
{
	
	// TODO: write code for proper thread

	return (NULL);
}

static void * readingThread (void * arg)
{
	// TODO: write code for proper thread

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

void cleanUp()
{
	// TODO: implement cleanup to properly delete pointers
}