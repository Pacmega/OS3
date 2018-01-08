#include <libusb-1.0/libusb.h>
#include <sys/fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <string.h>
#include <mqueue.h>
#include <unistd.h>
#include <signal.h>     // For the keyboard interrupt
#include <stdbool.h>    // Booleans
#include <stdlib.h>

#include "../lib/structs.h" // structs that we will use to communicate to daemon
#include "../lib/semshm.h"

#define MEMORYSIZE 5 // The size of the shared memory

bool interruptDetected = false;

void printInput(inputStruct* input);
void InterruptHandler(int sig);
void safeExit (sem_t * semaphoreToTest);
void endPage();
void outputsForm();
void startPage(char* title);
void header(char* mimeType);
void reading (char* memoryName, sem_t* writeSem, sem_t* readSem);


void handleInput()
{
	char* data;
	long lr, rr, l;		// The values for the left- and right rumbler and light

	data = getenv("QUERY_STRING");
	if (data == NULL)
		printf("No message found\n");
	else if (sscanf(data, "lr=%ld&rr=%ld&l%ld", &lr, &rr, &l) != 3)	// Check if there are 3 variables sent to the cgi
		printf("Invalid data. Data must be numeric.\n");
	else
	{
		char mq_name[80] = "/_mq_mq_";
		int rtnval = -1;

		mqd_t mq_fd = -1;
		struct mq_attr attr;

		x360outputs outputs;
		outputs.leftRumbler = lr;
		outputs.rightRumbler = rr;
		outputs.lightFunction = l;

		mq_fd = mq_open(mq_name, O_WRONLY);	// Try to open an existing MQ (Write only mode)

		if (mq_fd != -1)	// If it exists
		{
			// Send the outputs to the queue
			rtnval = mq_send(mq_fd, (char *) &outputs, sizeof(outputs), 0);
			if (rtnval == -1)
				printf("Error sending message to queue.\n");

		}
		else // Else: open a new MQ
		{
			attr.mq_maxmsg = 3;	// We only need 1 struct at the time, but this is for convenience 
			attr.mq_msgsize = sizeof(x360outputs);
			
			rtnval = mq_fd = mq_open(mq_name, O_WRONLY | O_CREAT | O_EXCL, 0600, &attr);
			if (rtnval == -1)
			{
				printf("Error opening a new queue.\n");
				return;
			}

			rtnval = mq_send(mq_fd, (char *) &outputs, sizeof(outputs), 0);

			if (rtnval == -1)
				printf("Error sending message to queue.\n");
		}

		rtnval = mq_unlink(mq_name);

		if (rtnval == -1)
			printf("Error unlinking from the queue.\n");	
	}
}

void reading (char* memoryName,
	 			sem_t* writeSem, 
	 			sem_t* readSem)
{
    int rtnval;
    int positionToRead;
    inputStruct readInput;

    while(!interruptDetected)
    {
        rtnval = sem_wait(writeSem);
        if(rtnval != 0)
        {
            perror("ERROR: sem_wait() failed");
            break;
        }

        // Get the position where there is space to write.
        rtnval = sem_getvalue(writeSem, &positionToRead);
        if(rtnval != 0)
        {
            perror("ERROR: sem_getvalue() failed");
            break;
        }

        // TODO: actually use the read values
        // inputStruct readInput = shm_inputs[positionToRead];
        
        rtnval = sem_post(readSem);
        if(rtnval != 0)
        {
            perror("ERROR: sem_post() failed");
            break;
        }
        // print inputstruct
    	inputStruct* shm_inputs = (inputStruct*)memoryName;

    	printInput(shm_inputs);

        printf("\n");
    }

    // Check if the readSem semaphore is 0 and if so post to it, to avoid the other thread getting a deadlock.
    safeExit(readSem);
}

// HTML stuff

void header(char* mimeType)
{
	printf("Content-type:%s\n\n", mimeType);
}



void startPage(char* title)
{
	printf("<html>");
	printf("<head>");
	printf("<title>%s</title>", title);
	printf("</head>");
	printf("<body>");
}

void outputsForm()
{
	printf("<form action='../mult.cgi'>");
		printf("<div>");
			printf("<label>");
				printf("Left rumbler intensity (0 to 100%%)");
				printf("<input name='lr' size='3'>");
			printf("</label>");
		printf("</div>");
		printf("<div>");
			printf("<label>");
				printf("Right rumbler intensity (0 to 100%%)");
				printf("<input name='rr' size='3'>");
			printf("</label>");
		printf("</div>");


		printf("<div>");
			printf("<label>");
				printf("Light (0 to 13): <input name='l' size='2'>");
			printf("</label>");
		printf("</div>");
		printf("<div>");
			printf("<input type='submit' value='Send'>");
		printf("</div>");
	printf("</form>");


	/* Probably not needed, but i'll keep this here for now
	<form action="../mult.cgi">
		<div>
			<label>
				Update controller-input:
				<input type="submit" value="Send">	
			</label>
			
		</div>
	</form>*/
}

void endPage()
{
	printf("</body>");
	printf("</html>");
}

// Handling a clean exit for the semaphore

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

// Handling the interrupt

void InterruptHandler(int sig)
{
    signal(sig, SIG_IGN);
    interruptDetected = true;
}

void printInput(inputStruct* input)
{
	printf("This will be the input\n");
}


int main(void)
{
	// Set up the interrupt for pressing ctrl-C, so it doesn't kill the program.
    // Instead, the program starts closing the semaphore & shared memory.
    signal(SIGINT, InterruptHandler);

    char* memoryName;		// No names needed, they are pre-defined in semshm.h
    sem_t* writeSem;			
    sem_t* readSem;

    // Other variables that are used throughout the program.
    int structSize = sizeof(inputStruct);
    int arraySize = MEMORYSIZE * structSize;

    memoryName = my_shm_create(arraySize, sharedMemName);
    writeSem = my_sem_open(writeSemName);
    readSem = my_sem_open(readSemName);

	// Starting to create the HTML:
	header("text/html");
	startPage("Controller Controller");

	printf("<p>Fill this in to send output to the controller</p>\n");
	
	outputsForm();

	printf("<p>If something went wrong with sending the output, It'll be shown below:</p>\n");

	handleInput();

	printf("<p>Here, the input of the controller is read and displayed below. You can stop getting data using CTRL+C</p>\n");
	
	if (writeSem == SEM_FAILED)
	{
		printf("writeSem semaphore failed to open. \n");
		interruptDetected = true;
	}
	if (readSem == SEM_FAILED)
	{
		printf("readSem semaphore failed to open\n");
		interruptDetected = true;
	}

	while(!interruptDetected) // read shared memory while there is no keyboard input
	{// note: don't know if this works in a browser
		reading(memoryName, writeSem, readSem);
	}

	// safeExit(); for when the semaphores work

	printf("<p>You have clicked CTRL+C. The end of the page will now be made.</p>\n");

	endPage();

    shmCleanup(memoryName);
    semCleanup(writeSemName);
    semCleanup(readSemName);

    return 0;
}