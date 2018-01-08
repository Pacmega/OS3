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
#include "../lib/defines.h"
#include "../lib/xboxUSB.h"

void printInput(inputStruct* input);
void endPage();
void startPage(char* title);
void header(char* mimeType);
void reading (char* memoryName, sem_t* writeSem, sem_t* readSem);


void handleInput(int choice)
{
	char mq_name[nameSize] = messageQueueName;
	int rtnval = -1;

	mqd_t mq_fd = -1; 

	mq_fd = mq_open(mq_name, O_WRONLY);	// Try to open an existing MQ (Write only mode)

	if (mq_fd != -1)	// If it exists
	{
		// Send the outputs to the queue
		rtnval = mq_send(mq_fd, (char *) &choice, sizeof(choice), 0);
		if (rtnval == -1)
			printf("Error sending message to queue.\n");

		mq_unlink(mq_name);
	}
}

void reading(char* memoryName,
	sem_t* writeSem, 
	sem_t* readSem)
{
	int rtnval;
	int positionToRead;
	//inputStruct readInput;

	rtnval = sem_wait(writeSem);
	if(rtnval != 0)
	{
		perror("ERROR: sem_wait() failed");
		return;
	}

        // Get the position where there is space to write.
	rtnval = sem_getvalue(writeSem, &positionToRead);
	if(rtnval != 0)
	{
		perror("ERROR: sem_getvalue() failed");
		return;
	}

        // TODO: actually use the read values
        // inputStruct readInput = shm_inputs[positionToRead];

	rtnval = sem_post(readSem);
	if(rtnval != 0)
	{
		perror("ERROR: sem_post() failed");
		return;
	}
        // print inputstruct
	inputStruct* shm_inputs = (inputStruct*)memoryName;

	printInput(shm_inputs);

	printf("\n");

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

void endPage()
{
	printf("</body>");
	printf("</html>");
}

void printInput(inputStruct* input)
{
	printf("<p>Input: </p>\n");

	if (input->leftTriggerInput > deadzone)
	{
		printf("Left trigger pressed\n");
	}

	if (input->rightTriggerInput > deadzone)
	{
		printf("Right trigger pressed\n");
	}

	if (input->group1Input> 0)
	{
		// At least one of the buttons in this group is pressed

		if (input->group1Input >= rightStick)
		{
			input->group1Input -= rightStick;
			printf("Right stick pressed\n");
		}
		if (input->group1Input >= leftStick)
		{
			input->group1Input -= leftStick;
			printf("Left stick pressed\n");
		}
		if (input->group1Input >= backButton)
		{
			input->group1Input -= backButton;
			printf("Back pressed\n");
		}
		if (input->group1Input >= startButton)
		{
			input->group1Input -= startButton;
			printf("Start pressed\n");
		}
		if (input->group1Input >= dpadRight)
		{
			input->group1Input -= dpadRight;
			printf("D-Pad right pressed\n");
		}
		if (input->group1Input >= dpadLeft)
		{
			input->group1Input -= dpadLeft;
			printf("D-Pad left pressed\n");
		}
		if (input->group1Input >= dpadDown)
		{
			input->group1Input -= dpadDown;
			printf("D-Pad down pressed\n");
		}
		if (input->group1Input >= dpadUp)
		{
			input->group1Input -= dpadUp;
			printf("D-Pad up pressed\n");
		}
	}

	if (input->group2Input > 0)
	{
		// At least one of the buttons in this group is pressed
		if (input->group2Input >= buttonY)
		{
			input->group2Input -= buttonY;
			printf("Y ");
		}
		if (input->group2Input >= buttonX)
		{
			input->group2Input -= buttonX;
			printf("X ");
		}
		if (input->group2Input >= buttonB)
		{
			input->group2Input -= buttonB;
			printf("B ");
		}
		if (input->group2Input >= buttonA)
		{
			input->group2Input -= buttonA;
			printf("A ");
		}
		if (input->group2Input >= buttonXBOX)
		{
			input->group2Input -= buttonXBOX;
			printf("XBOX button pressed\n");
		}
		if (input->group2Input >= rightShoulder)
		{
			input->group2Input -= rightShoulder;
			printf("Right shoulder pressed\n");
		}
		if (input->group2Input >= leftShoulder)
		{
			input->group2Input -= leftShoulder;
			printf("Left shoulder pressed\n");
		}
	}
}

int main(void)
{
	char* memName = sharedMemName;
	char* availableName = itemAvailableSemName;
	char* requestName = itemRequestSemName;

	char* memory;
	sem_t* availableSem;			
	sem_t* requestSem;

    // Other variables that are used throughout the program.
	int structSize = sizeof(inputStruct);

	memory = my_shm_create(structSize, memName);
	requestSem = my_sem_open(requestName);
	availableSem = my_sem_open(availableName);

	// Starting to create the HTML:
	header("text/html\n");
	startPage("Controller Controller\n");

	char* data;
	int choice;

	printf("<p>Choose a number:</p>\n");
	printf("[0]		LEDs Blink\n"
		"[1]		LEDs Spin\n"
		"[2]		LEDs Off\n"
		"[3]		Left Rumbler on\n"
		"[4]		Right Rumbler on\n"
		"[5]		Left Rumbler off\n"
		"[6]		Right Rumbler off\n"
		"[7]		Read Controller input");
	
	data = getenv("QUERY_STRING");
	
	if (data == NULL)
		printf("No message found\n");
	else if (sscanf(data, "choice=%d", &choice) != 1)	// Check if there are 3 variables sent to the cgi
		printf("Invalid data. Data must be numeric.\n");
	else
	{
		if (choice == 7)
		{
			if (requestSem != SEM_FAILED && availableSem != SEM_FAILED)
			{
				reading(memory, requestSem, availableSem);
				handleInput(choice);
			}
		}
		else
		{
			handleInput(choice);
		}

		shmCleanup(memory);
		semCleanup(requestName);
		semCleanup(availableName);
	}

	endPage();

	return 0;
}
