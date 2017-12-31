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

#include "../lib/structs.h" // structs that we will use to communicate to daemon
#include "../lib/semshm.h"

#define MEMORYSIZE 5 // The size of the shared memory

void handleInput();
void reading (multithreading MTstruct);
void header(char* mimeType);
void startPage(char* title);
void outputsForm();
void endPage();
void safeExit (sem_t * semaphoreToTest);
void  InterruptHandler(int sig);

bool interruptDetected = false;

int main(void)
{
	// Set up the interrupt for pressing ctrl-C, so it doesn't kill the program.
    // Instead, the program starts closing the semaphore & shared memory.
    signal(SIGINT, InterruptHandler);

    // Setting up the variables to make the semaphore
    multithreading MTstruct;
    MTstruct.sharedMem = (char *) MAP_FAILED;
    MTstruct.itemsFilled = SEM_FAILED;
    MTstruct.spaceLeft = SEM_FAILED;

    char* memoryName = "inputStorage";
    char* itemsFilledSemName = "itemsFilled";
    char* spaceLeftSemName = "spaceLeft";

    // Other variables that are used throughout the program.
    int structSize = sizeof(x360inputs);
    int arraySize = MEMORYSIZE * structSize;

    MTstruct.sharedMem = my_shm_create(arraySize, memoryName);
    my_sem_open(&MTstruct.itemsFilled, itemsFilledSemName);
    my_sem_open(&MTstruct.spaceLeft, spaceLeftSemName);

	// Starting to create the HTML:
	header("text/html");
	startPage("Controller Controller");

	printf("<p>Fill this in to send output to the controller</p>\n");
	
	outputsForm();

	printf("<p>If something went wrong with sending the output, It'll be shown below:</p>\n");

	handleInput();

	printf("<p>Here, the input of the controller is read and displayed below. You can stop getting data using CTRL+C</p>\n");
	
	if (MTstruct.itemsFilled == SEM_FAILED)
	{
		printf("itemsFilled semaphore failed to open. \n");
		interruptDetected = true;
	}
	if (MTstruct.spaceLeft == SEM_FAILED)
	{
		printf("spaceLeft semaphore failed to open\n");
		interruptDetected = true;
	}

	while(!interruptDetected) // read shared memory while there is no keyboard input
	{// note: don't know if this works in a browser
		reading(MTstruct);
	}

	// safeExit(); for when the semaphores work

	printf("<p>You have clicked CTRL+C. The end of the page will now be made.</p>\n");

	endPage();

    shmCleanup(memoryName);
    semCleanup(itemsFilledSemName);
    semCleanup(spaceLeftSemName);

    return 0;
}

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

void reading (multithreading MTstruct)
{
    int rtnval;
    int positionToRead;
    // x360inputs readNr;

    // TODO: make use of this
    // x360inputs* shm_inputs = (x360inputs*)MTstruct.sharedMem;

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

        // TODO: actually use the read values
        // x360inputs readInput = shm_inputs[positionToRead];
        
        rtnval = sem_post(MTstruct.spaceLeft);
        if(rtnval != 0)
        {
            perror("ERROR: sem_post() failed");
            break;
        }

        // printf("input: %d - ", readNr.value);

        	// To DO: convert to print x360inputs
/*        int i = 0;
        for (; i < PRONUNCIATIONLENGTH; i++)
        {
            char gelezenKarakter = readNr.pronunciation[i];
            if (gelezenKarakter == '\0')
            {
                break;
            }
            else
            {
                printf("%c", gelezenKarakter);
            }
        }*/
        printf("\n");
    }

    // Check if the spaceLeft semaphore is 0 and if so post to it, to avoid the other thread getting a deadlock.
    safeExit(MTstruct.spaceLeft);
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

void  InterruptHandler(int sig)
{
    signal(sig, SIG_IGN);
    interruptDetected = true;
}