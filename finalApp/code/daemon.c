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

// Create an input report array of 14 characters, all containing zeroes.
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

	// TODO: source lists two empty SIG handlers here. Needed or not?
	// (https://stackoverflow.com/questions/17954432/creating-a-daemon-in-linux)

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

int main(int argc, char const *argv[])
{
	createDaemon();

    libusb_device_handle *h;
	
	int error, transferred;
	libusb_init(NULL);
	h = libusb_open_device_with_vid_pid(NULL, 0x045e, 0x028e);

	if (h == NULL)
	{
		fprintf(stderr, "Failed to open device\n");
		return (1);
	}

	// inputStruct structToSend;
	
	error = 0;

	// TODO: receive USB data on demand instead of continuously
	while(true)
	{
		if ((error = libusb_interrupt_transfer(h, 0x81, inputReport, sizeof(inputReport), &transferred, 0)) != 0)
		{
			fprintf(stderr, "Transfer failed: %d\n", error);
			return (1);
		}
		else
		{
			printReport();
			// interpretButtons();
			// rumbleSetting(smallRumbler, bigRumbler);
			// lightSetting(lightMode);

			// if ((error = sendNewSettings(h, lightMode, smallRumbler, bigRumbler)) != 0)
			// {
			// 	fprintf(stderr, "Transfer failed: %d\n", error);
			// 	return (1);
			// }
		}
	}

    return 0;
}