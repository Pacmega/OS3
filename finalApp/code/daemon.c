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
#include "../lib/auxiliary.h"  // TODO: describe what this header is useful for

#include "../lib/semshm.h"     // Semaphore & shared memory management functions
#include "../lib/xboxUSB.h"    // Xbox 360 USB information and _BV() macro
#include "../lib/defines.h"		   // defined names for semaphores, shared memory & message queue

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

// TODO: delete this
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

int createStick(unsigned char leftMost, unsigned char secondLeft, unsigned char secondRight, unsigned char rightMost)
{
	int stickValue = 0;
	stickValue |= (leftMost << (3 * sizeof(unsigned char)));
	stickValue |= (secondLeft << (2 * sizeof(unsigned char)));
	stickValue |= (secondRight << (1 * sizeof(unsigned char)));
	stickValue |= rightMost;
	return stickValue;
}

void * settingChanger (void* arg)
{
	multithreading * mtStruct = (multithreading*) arg;
	int rtnval = 0;
	char receivedMessage;
	sem_post(mtStruct->itemRequestedSem);

	while(1)
	{
		printf("%p\n", &receivedMessage);
		rtnval = mq_receive(mtStruct->messageQueue, &receivedMessage, sizeof(receivedMessage), NULL);
		if(rtnval == -1)
        {
            perror("ERROR: mq_receive() failed");
            break;
        }
        else
        {
        	printf("Received: %c (%d).\n", receivedMessage, receivedMessage);
        }

        switch(receivedMessage)
        {
        	case 0:
        		// Idk, what does this do?
        		break;
        	case 1:
        		// Idk, what does this do?
        		break;
        	case 2:
        		// Idk, what does this do?
        		break;
        	case 3:
        		// Idk, what does this do?
        		break;
        	case 4:
        		// Idk, what does this do?
        		break;
        	case 5:
        		// Idk, what does this do?
        		break;
        	case 6:
        		// Idk, what does this do?
        		break;
        	default:
        		// This is not a value the USB daemon should have to deal with.
        		break;
        }

        // TODO: use received message to change setting on controller
        printf("Mate what\n");
	}
	
	return (NULL);
}

void * inputReporter (void* arg)
{
	// multithreading * mtStruct = (multithreading*) arg;
	// inputStruct structToSend;
	// int rtnval = 0;
	// int transferred;

	// inputStruct* shm_inputStruct = (inputStruct*) mtStruct->sharedMemory;

	// while(true)
	// {
	// 	rtnval = sem_wait(mtStruct->itemRequestedSem);
	// 	if(rtnval != 0)
 //        {
 //            perror("ERROR: sem_wait() failed");
 //            break;
 //        }

	// 	// TODO: uncomment this
	// 	if ((rtnval = libusb_interrupt_transfer(mtStruct->deviceHandle, 0x81, inputReport, sizeof(inputReport), &transferred, 0)) != 0)
	// 	{
	// 		fprintf(stderr, "Transfer failed: %d\n", rtnval);
	// 		break;
	// 	}
	// 	else
	// 	{
	// 		printReport();
	// 		structToSend.group1Input = inputReport[inputGroup1];
	// 		structToSend.group2Input = inputReport[inputGroup2];
	// 		structToSend.leftTriggerInput = inputReport[leftTrigger];
	// 		structToSend.rightTriggerInput = inputReport[rightTrigger];
	// 		structToSend.leftStickInput = createStick(inputReport[leftStickPt1], inputReport[leftStickPt2], inputReport[leftStickPt3], inputReport[leftStickPt4]);
	// 		structToSend.rightStickInput = createStick(inputReport[rightStickPt1], inputReport[rightStickPt2], inputReport[rightStickPt3], inputReport[rightStickPt4]);

	// 		*shm_inputStruct = structToSend;
	// 		// interpretButtons();
	// 		// rumbleSetting(smallRumbler, bigRumbler);
	// 		// lightSetting(lightMode);

	// 		// if ((rtnval = sendNewSettings(deviceHandle, lightMode, smallRumbler, bigRumbler)) != 0)
	// 		// {
	// 		// 	fprintf(stderr, "Transfer failed: %d\n", rtnval);
	// 		// 	return (1);
	// 		// }
	// 	}

	// 	rtnval = sem_post(mtStruct->itemAvailableSem);
	// 	if(rtnval != 0)
 //        {
 //            perror("ERROR: sem_post() failed");
 //            break;
 //        }
	// }

	return (NULL);
}

int main(int argc, char const *argv[])
{
	// TODO: once everything works, create as daemon instead of normal process
	// TODO: to translate to daemon language, replace all prints by syslog(LOG_NOTICE, *message string*);
	// createDaemon();
	
	multithreading mtStruct;
	mtStruct.messageQueue = -1;
	mtStruct.itemAvailableSem = my_sem_open(itemAvailableSemName); // Create the semaphore to publish if an item was already provided by this program.
	mtStruct.itemRequestedSem = my_sem_open(itemRequestSemName);   // Create the semaphore to know if an item was requested by the user
	mtStruct.sharedMemory     = (char *) MAP_FAILED;
	pthread_t changeSettingsThread;
	pthread_t sendStatusThread;

	int structSize = sizeof(inputStruct);

	libusb_init(NULL);
	// TODO: uncomment this
	// mtStruct.deviceHandle = libusb_open_device_with_vid_pid(NULL, 0x045e, 0x028e);

	// TODO: uncomment this
	// if (mtStruct.deviceHandle == NULL)
	// {
	// 	fprintf(stderr, "Failed to open device\n");
	// 	return (1);
	// }

	mtStruct.messageQueue = mq_open (messageQueueName, O_RDONLY);
	if (mtStruct.messageQueue == -1)
	{
		struct mq_attr attr;
		attr.mq_maxmsg = 1;
		attr.mq_msgsize = sizeof(int);

		mtStruct.messageQueue = mq_open (messageQueueName, O_RDONLY | O_CREAT | O_EXCL, 0600, &attr);
		
		if (mtStruct.messageQueue == -1)
		{
			// Messagequeue is unusable.
			printf("Unable to open message queue.");
			exit(EXIT_FAILURE);
		}
	}

	if (mtStruct.itemAvailableSem == SEM_FAILED)
    {
        printf("Critical error: unable to open itemsFilled semaphore.");

        // Unable to properly execute without semaphore, shut down.
        exit(EXIT_FAILURE);
    }
    if (mtStruct.itemRequestedSem == SEM_FAILED)
    {
        printf("Critical error: unable to open spaceLeft semaphore.");

        // Unable to properly execute without semaphore, shut down.
        exit(EXIT_FAILURE);
    }

    if (mtStruct.sharedMemory == MAP_FAILED)
    {
        mtStruct.sharedMemory = my_shm_open(structSize, sharedMemName);
        if (mtStruct.sharedMemory == MAP_FAILED)
        {
            printf("Critical error: unable to open or create shared memory.\n");

            // Unable to properly execute without shared memory, shut down.
            exit(EXIT_FAILURE);
        }
    }

	if (pthread_create (&changeSettingsThread, NULL, settingChanger, &mtStruct) != 0)
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

    exit(EXIT_SUCCESS);
}