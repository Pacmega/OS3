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
void reading ();


void handleInput(char choice)
{
    char mq_name[nameSize] = messageQueueName;
    int rtnval = -1;

    mqd_t mq_fd = mq_open(mq_name, O_WRONLY); // Try to open an existing MQ (Write only mode)

    if (mq_fd != -1)    // if the queue opened
    {
        // Send the outputs to the queue

        rtnval = mq_send(mq_fd, &choice, sizeof(char), 0);
        if (rtnval == -1)
        {
            perror("Error sending message to queue");
        }
        else
        {
        	printf("Message sent.\n");
        }

        // It is opened now, let's close it.
        mq_close(mq_fd);
    }
    else
    {
    	printf("Critical error: unable to open message queue, can't send command to controller.\n");
    }
    // if the queue didn't open, it means the daemon isnt running correctly
}

void reading()
{
    int structSize = sizeof(inputStruct);

    sem_t* availableSem = my_sem_open(itemAvailableSemName);
    sem_t* requestSem 	= my_sem_open(itemRequestSemName);
    char* memory 		= my_shm_open(structSize, sharedMemName);
    
    int rtnval;
    
    if (availableSem == SEM_FAILED)
    {
        printf("Critical error: unable to open \"items available\" semaphore.");
        // Unable to properly execute without semaphore, return.
        return;
    }
    if (requestSem == SEM_FAILED)
    {
        printf("Critical error: unable to open \"item requesting\" semaphore.");
        // Unable to properly execute without semaphore, return.
        return;
    }

    if (memory == MAP_FAILED)
    {
        printf("Critical error: unable to open or create shared memory.\n");
        // Unable to properly execute without shared memory, return.
        return;
    }

    // Request an input report from the controller.
    rtnval = sem_post(requestSem);
    if(rtnval != 0)
    {
        perror("ERROR: sem_post() failed");
        return;
    }

    // Wait until the daemon reports that an item is available.
    rtnval = sem_wait(availableSem);
    if(rtnval != 0)
    {
        perror("ERROR: sem_wait() failed");
        return;
    }

    // An input report is now available in the shared memory, so take it from there.
    inputStruct* shm_inputs = (inputStruct*)memory;

    if (shm_inputs == NULL)
    {
    	printf("Unable to get input report from shared memory.\n");
    }
    else
    {
    	printInput(shm_inputs);
    }

    shmCleanup(memory);
    sem_close(availableSem);
    sem_close(requestSem);
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
    printf("<p>Input: </p>");
    printf("<ul>");

    if (input->leftTriggerInput > deadzone)
    {
        printf("<li>Left trigger pressed</li>");
    }

    if (input->rightTriggerInput > deadzone)
    {
        printf("<li>Right trigger pressed</li>");
    }

    if (input->group1Input> 0)
    {
        // At least one of the buttons in this group is pressed

        if ((input->group1Input & _BV(rightStick)) == rightStickPressed)
        {
            printf("<li>Right stick pressed</li>");
        }
        if ((input->group1Input & _BV(leftStick)) == leftStickPressed)
        {
            printf("<li>Left stick pressed</li>");
        }
        if ((input->group1Input & _BV(backButton)) == backButtonPressed)
        {
            printf("<li>Back pressed</li>");
        }
        if ((input->group1Input & _BV(startButton)) == startButtonPressed)
        {
            printf("<li>Start pressed</li>");
        }
        if ((input->group1Input & _BV(dpadRight)) == dpadRightPressed)
        {
            printf("<li>D-Pad right pressed</li>");
        }
        if ((input->group1Input & _BV(dpadLeft)) == dpadLeftPressed)
        {
            printf("<li>D-Pad left pressed</li>");
        }
        if ((input->group1Input & _BV(dpadDown)) == dpadDownPressed)
        {
            printf("<li>D-Pad down pressed</li>");
        }
        if ((input->group1Input & _BV(dpadUp)) == dpadUpPressed)
        {
            printf("<li>D-Pad up pressed</li>");
        }
    }

    if (input->group2Input > 0)
    {
        // At least one of the buttons in this group is pressed
        if ((input->group2Input & _BV(buttonY)) == buttonYPressed)
        {
            printf("<li>Y pressed</li>");
        }
        if ((input->group2Input & _BV(buttonX)) == buttonXPressed)
        {
            printf("<li>X pressed</li>");
        }
        if ((input->group2Input & _BV(buttonB)) == buttonBPressed)
        {
            printf("<li>B pressed</li>");
        }
        if ((input->group2Input & _BV(buttonA)) == buttonAPressed)
        {
            printf("<li>A pressed</li>");
        }

        if ((input->group2Input & _BV(buttonXBOX)) == buttonXBOXPressed)
        {
            printf("<li>XBOX button pressed</li>");
        }
        if ((input->group2Input & _BV(rightShoulder)) == rightShoulderPressed)
        {
            printf("<li>Right shoulder pressed</li>");
        }
        if ((input->group2Input & _BV(leftShoulder)) == leftShoulderPressed)
        {
            printf("<li>Left shoulder pressed</li>");
        }
    }

    printf("</ul>");
}

int main(void)
{

    // Starting to create the HTML:
    header("text/html\n");
    startPage("Controller Controller\n");

    char* data;
    char choice;

    printf("<p>Choose a number:</p>\n");
    printf("<p>[0]      LEDs Blink </p>"
           "<p>[1]     LEDs Spin</p>"
           "<p>[2]     LEDs Off</p>"
           "<p>[3]     Left Rumbler on</p>"
           "<p>[4]     Left Rumbler off</p>"
           "<p>[5]     Right Rumbler on</p>"
           "<p>[6]     Right Rumbler off</p>"
           "<p>[7]     Read Controller input</p>");
    
    data = getenv("QUERY_STRING");
    
    if (data == NULL)
    {
        printf("No message found\n");
    }
    else if (sscanf(data, "choice=%c", &choice) != 1)  // Check if there is a variable sent to the cgi
    {
        printf("Invalid data. Data must be numeric.\n");
    }
    else
    {
    	if (choice == '0' || choice == '1' || choice == '2' || choice == '3' || choice == '4' || choice == '5' || choice == '6')
    	{
    		handleInput(choice);
    	}
    	else if (choice == '7')
        {
            reading();
        }
        else
        {
        	printf("Invalid data. Data must be numeric.\n");
        }
    }

    endPage();

    return 0;
}   
