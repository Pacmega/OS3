#include <stdio.h>             // Printing to the terminal
#include <string.h>            // Strings
#include <syslog.h>			   // Logging information
#include <stdlib.h>
#include <sys/types.h>         // Shared memory
#include <sys/mman.h>          // Shared memory
#include <sys/stat.h>          // Shared memory
#include <sys/fcntl.h>         // Shared memory
#include <semaphore.h>         // For POSIX semaphores
#include <pthread.h>           // For POSIX threads
#include <mqueue.h>            // For POSIX message queues
#include <stdbool.h>           // Booleans
#include <unistd.h>            // Sleep() & fork()
#include <signal.h>            // For the keyboard interrupt
#include <libusb-1.0/libusb.h> // USB library

// TODO: create a more useful structs file if it's even needed
#include "../lib/structs.h"    // Data about the multithreading and number structs

// TODO: go through this file and clean it up (and understand it)
// TODO: rename this file
#include "../lib/auxiliary.h"  // TODO: describe what this header is useful for

#include "../lib/semshm.h"     // Semaphore & shared memory management functions
#include "../lib/xboxUSB.h"    // Xbox 360 USB information and _BV() macro
#include "../lib/defines.h"		   // defined names for semaphores, shared memory & message queue

typedef struct
{
	char * 	sharedMemory;
	sem_t * itemAvailableSem;
	sem_t * itemRequestedSem;
	libusb_device_handle * deviceHandle;
} multithreading;

unsigned char inputReport[14] = {0};

// TODO: uncomment this
// static void createDaemon()
// {
// 	pid_t pid;

// 	// Fork off the parent process
// 	pid = fork();

// 	if (pid < 0)
// 	{
// 		// An error occured.
// 		exit(EXIT_FAILURE);
// 	}

// 	if (pid > 0)
// 	{
// 		// Process was successfully forked: parent can terminate.
// 		exit(EXIT_SUCCESS);
// 	}

// 	if (setsid() < 0)
// 	{
// 		// An error occured.
// 		exit(EXIT_FAILURE);
// 	}

// 	// TODO: source lists two empty SIG handlers here. Needed or not?
// 	// (https://stackoverflow.com/questions/17954432/creating-a-daemon-in-linux)

// 	// Fork off for the second time.
// 	if (pid < 0)
// 	{
// 		exit(EXIT_FAILURE);
// 	}

// 	if (pid > 0)
// 	{
// 		// Second fork succeeded: let this parent terminate as well.
// 		exit(EXIT_SUCCESS);
// 	}

// 	// File permissions for this process: none.
// 	umask(0);

// 	// Change the working directory of this process to the root directory
// 	if(chdir("/") != 0)
// 	{
// 		// Unable to change working directory.
// 		exit(EXIT_FAILURE);
// 	}

// 	// Close all opened file desriptors (STDIN, STDOUT)
// 	int x;
// 	for (x = sysconf(_SC_OPEN_MAX); x >= 0; x--)
// 	{
// 		close(x);
// 	}

// 	// Open a log file to send log messages to.
// 	openlog("USBdaemon", LOG_PID, LOG_DAEMON);
// }

void printReport()
{
	printf("Input report\n");
	printf("Message type\t\t%d\n", inputReport[messageType]);
	printf("Packet size (B)\t\t%d\n", inputReport[packetSize]);
	printf("D-Pad + button group 1\t%d\n", inputReport[inputGroup1]);
	printf("(Start/Back & stick press)\n");
	printf("Button group 2\t\t%d\n", inputReport[inputGroup2]);
	printf("(Shoulders, A/B/X/Y, Xbox logo)\n");
	printf("Left trigger\t\t%d\n", inputReport[leftTrigger]);
	printf("Right trigger\t\t%d\n", inputReport[rightTrigger]);
	
	printf("Left stick values\n");
	printf("%d\n", inputReport[6]);
	printf("%d\n", inputReport[7]);
	printf("%d\n", inputReport[8]);
	printf("%d\n", inputReport[9]);
	
	printf("Right stick values\n");
	printf("%d\n", inputReport[10]);
	printf("%d\n", inputReport[11]);
	printf("%d\n", inputReport[12]);
	printf("%d\n", inputReport[13]);
}

void * settingChanger (void* arg)
{
	printf("Ik ben een thread.\n");
	return (NULL);
}

void * inputReporter (void* arg)
{
	// TODO: uncomment this
	// libusb_device_handle* deviceHandle = (libusb_device_handle*) arg;
	// int rtnval = 0;
	// int transferred;

	// TODO: receive USB data on demand instead of continuously
	while(true)
	{
		// TODO: uncomment this
		// if ((rtnval = libusb_interrupt_transfer(deviceHandle, 0x81, inputReport, sizeof(inputReport), &transferred, 0)) != 0)
		// {
		// 	fprintf(stderr, "Transfer failed: %d\n", rtnval);
		// 	break;
		// }
		// else
		{
			printReport();
			// interpretButtons();
			// rumbleSetting(smallRumbler, bigRumbler);
			// lightSetting(lightMode);

			// if ((rtnval = sendNewSettings(deviceHandle, lightMode, smallRumbler, bigRumbler)) != 0)
			// {
			// 	fprintf(stderr, "Transfer failed: %d\n", rtnval);
			// 	return (1);
			// }
		}
	}

	return (NULL);
}

int main(int argc, char const *argv[])
{
	// TODO: once finished, turn into daemon instead of normal program
	// TODO: replace printf's by actual log messages
	// createDaemon();

	mqd_t messageQueue = -1;
	inputStruct structToSend;
	multithreading mtStruct;
	mtStruct.itemAvailableSem = my_sem_open(itemAvailableSemName); // Create the semaphore to publish if an item was already provided by this program.
	mtStruct.itemRequestedSem = my_sem_open(itemRequestSemName);   // Create the semaphore to know if an item was requested by the user
	mtStruct.sharedMemory     = (char *) MAP_FAILED;
	pthread_t changeSettingsThread;
	pthread_t sendStatusThread;

	int structSize = sizeof(structToSend);

	libusb_init(NULL);
	// TODO: uncomment this
	// mtStruct.deviceHandle = libusb_open_device_with_vid_pid(NULL, 0x045e, 0x028e);

	// TODO: uncomment this
	// if (mtStruct.deviceHandle == NULL)
	// {
	// 	fprintf(stderr, "Failed to open device\n");
	// 	return (1);
	// }

	messageQueue = mq_open (messageQueueName, O_RDONLY);
	if (messageQueue == -1)
	{
		messageQueue = mq_open (messageQueueName, O_RDONLY | O_CREAT | O_EXCL, 0600, &structToSend);
		if (messageQueue == -1)
		{
			// Messagequeue is unusable.
			printf("Unable to open message queue.");
		}
	}

	if (mtStruct.itemAvailableSem == SEM_FAILED)
    {
        printf("Critical error: unable to open itemsFilled semaphore.");

        // Unable to properly execute without semaphore, shut down.
        return -1;
    }
    if (mtStruct.itemRequestedSem == SEM_FAILED)
    {
        printf("Critical error: unable to open spaceLeft semaphore.");

        // Unable to properly execute without semaphore, shut down.
        return -1;
    }

    if (mtStruct.sharedMemory == MAP_FAILED)
    {
        mtStruct.sharedMemory = my_shm_open(structSize, sharedMemName);
        if (mtStruct.sharedMemory == MAP_FAILED)
        {
            printf("Critical error: unable to open or create shared memory.\n");

            // Unable to properly execute without shared memory, shut down.
            return -1;
        }
    }

	if (pthread_create (&changeSettingsThread, NULL, settingChanger, NULL) != 0)
    {
        perror ("controller setting changing thread");
    }
    if (pthread_create (&sendStatusThread, NULL, inputReporter, &mtStruct) != 0)
    {
        perror ("status submitting thread");
    }

    pthread_join(changeSettingsThread, NULL);
    pthread_join(sendStatusThread, NULL);

    printf("Shutting down daemon.\n");

	shmCleanup(sharedMemName);
    semCleanup(itemAvailableSemName);
    semCleanup(itemRequestSemName);

    return 0;
}