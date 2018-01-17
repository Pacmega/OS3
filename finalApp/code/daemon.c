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

#include "../lib/structs.h"    // Data about the multithreading and number structs

// TODO: go through this file and clean it up (and understand it)
#include "../lib/auxiliary.h"  // TODO: describe what this header is useful for

#include "../lib/semshm.h"     // Semaphore & shared memory management functions
#include "../lib/xboxUSB.h"    // Xbox 360 USB information and _BV() macro
#include "../lib/defines.h"		   // defined names for semaphores, shared memory & message queue

unsigned char inputReport[14] = {0};

static void createDaemon()
{
	pid_t pid;

	// Fork off the parent process
	pid = fork();

	if (pid < 0)
	{
		// An error occured.
		exit(EXIT_FAILURE);
	}

	if (pid > 0)
	{
		// Process was successfully forked: parent can terminate.
		exit(EXIT_SUCCESS);
	}

	if (setsid() < 0)
	{
		// An error occured.
		exit(EXIT_FAILURE);
	}

	// Fork off for the second time.
	if (pid < 0)
	{
		exit(EXIT_FAILURE);
	}

	if (pid > 0)
	{
		// Second fork succeeded: let this parent terminate as well.
		exit(EXIT_SUCCESS);
	}

	// File permissions for this process: none.
	umask(0);

	// Change the working directory of this process to the root directory
	if(chdir("/") != 0)
	{
		// Unable to change working directory.
		exit(EXIT_FAILURE);
	}

	// Close all opened file desriptors (STDIN, STDOUT)
	int x;
	for (x = sysconf(_SC_OPEN_MAX); x >= 0; x--)
	{
		close(x);
	}

	// Open a log file to send log messages to.
	openlog("USBdaemon", LOG_PID, LOG_DAEMON);
}


// ads the 4 stickvalues into 1 int and returns the value
int createStick(unsigned char leftMost, unsigned char secondLeft, 
				unsigned char secondRight, unsigned char rightMost)
{
	int stickValue = 0;
	stickValue |= (leftMost << (3 * sizeof(unsigned char)));
	stickValue |= (secondLeft << (2 * sizeof(unsigned char)));
	stickValue |= (secondRight << (1 * sizeof(unsigned char)));
	stickValue |= rightMost;
	return stickValue;
}

// Reads the message queue and handles it accordingly
void * settingChanger (void* arg)
{
	multithreading * mtStruct = (multithreading*) arg;
	int rtnval, transferred;
	char receivedMessage;

	unsigned char lightMessage[3] = {1, 3, allLEDsOff};
	unsigned char rumbleMessage[8] = {0x00, 0x08, 0x00, noPower, noPower, 0x00, 0x00, 0x00};

	while(1)
	{
		rtnval = mq_receive(mtStruct->messageQueue, &receivedMessage, sizeof(receivedMessage), NULL);
		if(rtnval == -1)
        {
            syslog(LOG_ERR, "mq_receive() failed");
            break;
        }

        switch(receivedMessage)
        {
        	case 0: // command 0 sets all LEDs to blink
        		lightMessage[2] = allLEDsBlinking;
				rtnval = libusb_interrupt_transfer(mtStruct->deviceHandle, 0x01, lightMessage, sizeof(lightMessage), &transferred, 0);
        		break;
        	case 1: // command 1 sets LEds to rotate
        		lightMessage[2] = LEDRotating;
				rtnval = libusb_interrupt_transfer(mtStruct->deviceHandle, 0x01, lightMessage, sizeof(lightMessage), &transferred, 0);
        		break;
        	case 2: // command 2 sets all LEDs off
        		lightMessage[2] = allLEDsOff;
				rtnval = libusb_interrupt_transfer(mtStruct->deviceHandle, 0x01, lightMessage, sizeof(lightMessage), &transferred, 0);
        		break;
        	case 3: // command 3 turns the left rumbler on
        		rumbleMessage[3] = mediumPower;
				rtnval = libusb_interrupt_transfer(mtStruct->deviceHandle, 0x01, rumbleMessage, sizeof(rumbleMessage), &transferred, 0);
        		break;
        	case 4: // command 4 turns the left rumbler off
        		rumbleMessage[3] = noPower;
				rtnval = libusb_interrupt_transfer(mtStruct->deviceHandle, 0x01, rumbleMessage, sizeof(rumbleMessage), &transferred, 0);
        		break;
        	case 5: // command 5 turns the right rumbler on
        		rumbleMessage[4] = mediumPower;
				rtnval = libusb_interrupt_transfer(mtStruct->deviceHandle, 0x01, rumbleMessage, sizeof(rumbleMessage), &transferred, 0);
        		break;
        	case 6: // command 6 turns the right rumbler off
        		rumbleMessage[4] = noPower;
				rtnval = libusb_interrupt_transfer(mtStruct->deviceHandle, 0x01, rumbleMessage, sizeof(rumbleMessage), &transferred, 0);
        		break;
        	default: // all non-programmed numbers wont do anything
        		// This is not a value the USB daemon should have to deal with.
        		break;
        }
	}
	
	return (NULL);
}

void * inputReporter (void* arg)
{
	multithreading * mtStruct = (multithreading*) arg;
	inputStruct structToSend;

	int rtnval = 0;
	int transferred;

	inputStruct* shm_inputStruct = (inputStruct*) mtStruct->sharedMemory;

	while(true)
	{
		// wait for items to be requested
		rtnval = sem_wait(mtStruct->itemRequestedSem);
		if(rtnval != 0)
        {
            syslog(LOG_ERR, "sem_wait failed.");
            break;
        }
        // If the transfer of the info on the controller failed, report it and stop
		if ((rtnval = libusb_interrupt_transfer(mtStruct->deviceHandle, 0x81, inputReport, sizeof(inputReport), &transferred, 0)) != 0)
		{
			syslog(LOG_ERR, "libusb_interrupt_transfer failed");
			break;
		}
		else
		{
			structToSend.group1Input = inputReport[inputGroup1];
			structToSend.group2Input = inputReport[inputGroup2];

			structToSend.leftTriggerInput = inputReport[leftTrigger];
			structToSend.rightTriggerInput = inputReport[rightTrigger];

			structToSend.leftStickInput = createStick(inputReport[leftStickPt1], inputReport[leftStickPt2], inputReport[leftStickPt3], inputReport[leftStickPt4]);
			structToSend.rightStickInput = createStick(inputReport[rightStickPt1], inputReport[rightStickPt2], inputReport[rightStickPt3], inputReport[rightStickPt4]);

			*shm_inputStruct = structToSend;
		}
		// When the input is written into the shared memory, post to alert cgi
		rtnval = sem_post(mtStruct->itemAvailableSem);
		if(rtnval != 0)
        {
            syslog(LOG_ERR, "sem_post failed");
            break;
        }
	}

	return (NULL);
}

int main(int argc, char const *argv[])
{
	createDaemon();

	int structSize = sizeof(inputStruct);
	
	multithreading mtStruct;
	mtStruct.messageQueue = -1;
	mtStruct.itemAvailableSem = my_sem_open(itemAvailableSemName); // Create the semaphore to publish if an item was already provided by this program.
	mtStruct.itemRequestedSem = my_sem_open(itemRequestSemName);   // Create the semaphore to know if an item was requested by the user
	mtStruct.sharedMemory     = my_shm_open(structSize, sharedMemName); // Tries to open the shm. If it failes, it will create a new one.

	pthread_t changeSettingsThread;
	pthread_t sendStatusThread;

	// connect to the controller using libusb
	libusb_init(NULL);
	mtStruct.deviceHandle = libusb_open_device_with_vid_pid(NULL, 0x045e, 0x028e);

	if (mtStruct.deviceHandle == NULL)
	{
		syslog(LOG_ERR, "Failed to open device");
		return (1);
	}
	// Try to open the message queue in read-only-mode
	mtStruct.messageQueue = mq_open(messageQueueName, O_RDONLY);
	if (mtStruct.messageQueue == -1)
	{
		// If it fails to open, create a new one
		struct mq_attr attr;
		attr.mq_maxmsg = 1;
		attr.mq_msgsize = sizeof(char);

		mtStruct.messageQueue = mq_open(messageQueueName, O_RDONLY | O_CREAT | O_EXCL, 0600, &attr);
		
		if (mtStruct.messageQueue == -1)
		{
			// Messagequeue is unusable.
			syslog(LOG_ERR, "Unable to open message queue.");
			exit(EXIT_FAILURE);
		}
	}

	if (mtStruct.itemAvailableSem == SEM_FAILED)
    {
        syslog(LOG_ERR, "Could not open itemsAvailableSem.");

        // Unable to properly execute without semaphore, shut down.
        exit(EXIT_FAILURE);
    }
    if (mtStruct.itemRequestedSem == SEM_FAILED)
    {
        syslog(LOG_ERR, "Could not open itemRequestedSem.");

        // Unable to properly execute without semaphore, shut down.
        exit(EXIT_FAILURE);
    }

    if (mtStruct.sharedMemory == MAP_FAILED)
    {
        mtStruct.sharedMemory = my_shm_open(structSize, sharedMemName);
        if (mtStruct.sharedMemory == MAP_FAILED)
        {
            syslog(LOG_ERR, "Could not open sharedMemory.");

            // Unable to properly execute without shared memory, shut down.
            exit(EXIT_FAILURE);
        }
    }

	if (pthread_create (&changeSettingsThread, NULL, settingChanger, &mtStruct) != 0)
    {
        syslog(LOG_ERR, "controller setting changing thread");
    }
    if (pthread_create (&sendStatusThread, NULL, inputReporter, &mtStruct) != 0)
    {
        syslog(LOG_ERR, "status submitting thread");
    }

    pthread_join(changeSettingsThread, NULL);
    pthread_join(sendStatusThread, NULL);

    syslog(LOG_ERR, "Shutting down daemon.");

	shmCleanup(sharedMemName);
    semCleanup(itemAvailableSemName);
    semCleanup(itemRequestSemName);

    exit(EXIT_SUCCESS);
}