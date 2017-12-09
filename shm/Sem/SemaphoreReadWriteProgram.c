// Massive thanks to https://en.wikipedia.org/wiki/Producer%E2%80%93consumer_problem
// for giving a proper explanation of how this could be done using just two semaphores,
// since the powerpoint was unhelpful at best and confusing at worst.

#include <stdio.h>		// Printing to the terminal
#include <string.h>		// Strings
#include <sys/types.h>	// 
#include <sys/mman.h>	// 
#include <sys/stat.h>	// 
#include <sys/fcntl.h>	// 
#include <semaphore.h>	// Semaphores
#include <signal.h>		// For the keyboard interrupt
#include <stdbool.h>	// Booleans
#include <pthread.h>	// Threads
#include <unistd.h>     // Sleep()

#include "structs.h" 	// Data about the multithreading and number structs
#include "semshm.h" 	// Semaphore & shared memory management functions

#define NUMBERARRAYLENGTH 9

// This needs to be global for the interrupt handler to be able to change it.
bool interruptDetected = false;

static void * writingThread (void * arg);
static void * readingThread (void * arg);
void safeExit (sem_t * semaphoreToTest);

number createNr(int value, char* pronunciation);
void createStructs(number numberArray[]);
void  InterruptHandler(int sig);
void cleanUp();

int main(void)
{
	// Set up the interrupt for pressing ctrl-C, so it doesn't kill the program.
    // Instead, the program starts closes the semaphore & shared memory.
    signal(SIGINT, InterruptHandler);

	// Separate threads for reading and writing
	pthread_t   writeThread;
    pthread_t   readThread;

    // Semaphore and Shared Memory
    multithreading MTstruct;
    MTstruct.sharedMem = (char *) MAP_FAILED;
    MTstruct.itemsFilled = SEM_FAILED;
    MTstruct.spaceLeft = SEM_FAILED;

    char * memoryName = "OSassSharedMem";
    char * itemsFilledSemName = "itemsFilled";
    char * spaceLeftSemName = "spaceLeft";

    // Other variables that are used throughout the program.
    int         numberStructSize = sizeof(number);
    int         numberArraySize = NUMBERARRAYLENGTH * numberStructSize;

    // Names for both shared memory and the semaphore are set in semshm.c
    MTstruct.sharedMem = my_shm_open(memoryName);	// Open the shared memory and save the address
    my_sem_open(&MTstruct.itemsFilled, itemsFilledSemName); // Open the semaphore to count how many items are available
    my_sem_open(&MTstruct.spaceLeft, spaceLeftSemName);     // Open the semaphore to count how much space is left to write to

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
    	// Attempt to create the SHM instead of opening it

    	MTstruct.sharedMem = my_shm_create(numberArraySize, memoryName);

    	if (MTstruct.sharedMem == MAP_FAILED)
    	{
    		printf("Critical error: unable to open or create shared memory.\n");

    		// Unable to properly execute without shared memory, shut down.
    		return -1;
    	}
    }

    // Set the semaphore that counts the amount of available items to the right value.
    // Directly setting a semaphore is not possible, so we post several times.
    // int i = 0;
    // for (; i < NUMBERARRAYLENGTH; i++)
    // {
    //     int rtnval = sem_post(MTstruct.spaceLeft);
    //     if(rtnval != 0)
    //     {
    //         perror("Critical error: sem_post() failed while preparing spaceLeft semaphore.");
            
    //         // Unable to properly execute without proper buffer setup, shut down.
    //         return -1;
    //     }
    // }

    printf("\n\n----- PROGRAM STARTING READ AND WRITE THREADS -----\n\n");
    
    printf("Starting the writing and reading threads.\n");
    printf("To stop this process, press ctrl-C.\n");

    printf(  "\n---------------------------------------------------\n\n");
    sleep(5);

    // Create both threads
    if (pthread_create (&writeThread, NULL, writingThread, &MTstruct) != 0)
    {
        perror ("writing thread");
    }
    if (pthread_create (&readThread, NULL, readingThread, &MTstruct) != 0)
    {
        perror ("reading thread");
    }

    // Main does nothing except wait until the threads are joined again.
    // This is done by the interrupt that was set up earlier flipping a boolean that
    // tells both threads to shut down when true.

    (void) pthread_join(writeThread, NULL);
    (void) pthread_join(readThread, NULL);
    
    printf("Cleaning up shared memory and semaphores.\n");
    shmCleanup(memoryName);
    semCleanup(itemsFilledSemName);
    semCleanup(spaceLeftSemName);

    return 0;
}

// Code for both threads

static void * writingThread (void * arg)
{
	// Convert the given argument to a Multithreading pointer
	multithreading * MTstruct = (multithreading *) arg;

	int rtnval;
    int positionToWrite;
	int positionInNrArray = 0;
	number numberArray[9];
    createStructs(numberArray);

    number* shm_number = (number*)MTstruct->sharedMem;

    while(!interruptDetected)
    {
        // Wait until there is a spot available in the buffer to write to.
    	rtnval = sem_wait(MTstruct->spaceLeft);
        if(rtnval != 0)
        {
            perror("ERROR: sem_wait() failed");
            break;
        }

        // Get the position where there is space to write.
        rtnval = sem_getvalue(MTstruct->itemsFilled, &positionToWrite);
        if(rtnval != 0)
        {
            perror("ERROR: sem_getvalue() failed");
            break;
        }

        shm_number[positionToWrite] = numberArray[positionInNrArray];

        // Post that there is an item that could be read by the other thread.
		rtnval = sem_post(MTstruct->itemsFilled);
		if(rtnval != 0)
	    {
	        perror("ERROR: sem_post() failed");
	        break;
	    }

        // Change the integer that determines which number is written to the buffer.
        if(positionInNrArray < 8)
        {
            positionInNrArray++;
        }
        else
        {
            positionInNrArray = 0;
        }
    }

    // Check if the itemsFilled semaphore is 0 and if so post to it, to avoid the other thread getting a deadlock.
    safeExit(MTstruct->itemsFilled);

    printf("Writing thread reached end.\n");
	return (NULL);
}

static void * readingThread (void * arg)
{
	// Convert the given argument back to a Multithreading struct
	multithreading * MTstruct = (multithreading *) arg;

	int rtnval;
    int positionToRead;
	number readNr;

	number* shm_number = (number*)MTstruct->sharedMem;

	while(!interruptDetected)
    {
        rtnval = sem_wait(MTstruct->itemsFilled);
        if(rtnval != 0)
        {
            perror("ERROR: sem_wait() failed");
            break;
        }

        // Get the position where there is space to write.
        rtnval = sem_getvalue(MTstruct->itemsFilled, &positionToRead);
        if(rtnval != 0)
        {
            perror("ERROR: sem_getvalue() failed");
            break;
        }

        readNr = shm_number[positionToRead];
        
        rtnval = sem_post(MTstruct->spaceLeft);
        if(rtnval != 0)
        {
            perror("ERROR: sem_post() failed");
            break;
        }

        printf("[DBG] From pos %d: ", positionToRead);
        printf("%d - %s\n", readNr.value, readNr.pronunciation);
    }

    // Check if the spaceLeft semaphore is 0 and if so post to it, to avoid the other thread getting a deadlock.
    safeExit(MTstruct->spaceLeft);

    printf("Reading thread reached end.\n");
	return (NULL);
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
	// This function only works specifically with an array of 9 or more numbers.
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