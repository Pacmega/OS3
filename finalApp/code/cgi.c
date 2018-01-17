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
#include <stdbool.h>    // Booleans
#include <stdlib.h>

#include "../lib/structs.h" // structs that we will use to communicate to daemon
#include "../lib/semshm.h"
#include "../lib/defines.h"
#include "../lib/xboxUSB.h"

void printInput(inputStruct* input);
void readStick(int stickValue);
void endPage();
void startPage(char* title);
void header(char* mimeType);
void reading ();


void handleInput(int choice)
{
    char mq_name[nameSize] = messageQueueName;
    int rtnval = -1;
    // make a new message queue descriptor
    mqd_t mq_fd = -1; 

    mq_fd = mq_open(mq_name, O_WRONLY); // Try to open an existing MQ (Write only mode)

    if (mq_fd != -1)    // if the queue opened
    {
        // Send the outputs to the queue
        rtnval = mq_send(mq_fd, (char *) &choice, sizeof(char), 0);
        if (rtnval == -1)
            perror("<p>Error sending message to queue.</p>");

        mq_close(mq_fd);
    }
    else
        perror("<p>Queue didn't open. There could be something wrong with the daemon</p>");
}

void reading()
{
    int structSize = sizeof(inputStruct);
    // Open shared memory and semaphores to communicate with the daemon
    char* memory = my_shm_open(structSize, sharedMemName);
    sem_t* availableSem = my_sem_open(itemAvailableSemName);            
    sem_t* requestSem = my_sem_open(itemRequestSemName);

    // check if the semaphores and shared memory is opened correctly:
    if (availableSem == SEM_FAILED || requestSem == SEM_FAILED || memory == MAP_FAILED)
    {
        printf("<p>There were problems opening the semaphores or shared memory.</p>");
        return;
    }
    
    // request a report from the daemon
    int rtnval = sem_post(requestSem);
    if (rtnval != 0)
    {
        perror("sem_post failed");
        return;
    }

    // wait till the daemon is done writing the report to the memory
    rtnval = sem_wait(availableSem);
    if(rtnval != 0)
    {
        perror("ERROR: sem_wait() failed. Reading stopped.");
        return;
    }

    inputStruct* shm_inputs = (inputStruct*)memory;

    
    if(shm_inputs == NULL)
        perror("Something went wrong when reading the report from the memory");
    else
    {
        printInput(shm_inputs);
        // Close the memory and semaphores when done.
        shmCleanup(memory);
        sem_close(requestSem);
        sem_close(availableSem);
    }
}

// HTML stuff
// mimeType is a standardized way to indicate the nature and format of a doc 
// for more info: https://developer.mozilla.org/en-US/docs/Web/HTTP/Basics_of_HTTP/MIME_types
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

void readStick(int stickValue)
{
    unsigned char leftMost = (stickValue >> (3 * sizeof(unsigned char)));
    unsigned char secondLeft = (stickValue >> (2 * sizeof(unsigned char)) & sizeof(unsigned char));
    unsigned char rightMost = (stickValue >> (1 * sizeof(unsigned char)) & sizeof(unsigned char));
    unsigned char secondRight = (stickValue & sizeof(unsigned char));
    printf("<p>LeftMost: %c</p>", leftMost);
    printf("<p>secondLeft: %c</p>", secondLeft);
    printf("<p>rightMost: %c</p>", rightMost);
    printf("<p>secondRight: %c</p>", secondRight);
}

void printInput(inputStruct* input)
{
    printf("<h1>Input: </h1>");
/* Uncomment when problem found
    printf("<p>left stick:</p>");
    readStick(input->leftStickInput);
    printf("<p>right stick:</p>");
    readStick(input->rightStickInput); 
*/
    if (input->leftTriggerInput > deadzone)
    {
        printf("<p>Left trigger pressed</p>");
    }

    if (input->rightTriggerInput > deadzone)
    {
        printf("<p>Right trigger pressed</p>");
    }

    if (input->group1Input> 0)
    {
        // At least one of the buttons in this group is pressed

        if ((input->group1Input & _BV(rightStick)) == rightStickPressed)
        {
            printf("<p>Right stick pressed</p>");
        }
        if ((input->group1Input & _BV(leftStick)) == leftStickPressed)
        {
            printf("<p>Left stick pressed</p>");
        }
        if ((input->group1Input & _BV(backButton)) == backButtonPressed)
        {
            printf("<p>Back pressed</p>");
        }
        if ((input->group1Input & _BV(startButton)) == startButtonPressed)
        {
            printf("<p>Start pressed</p>");
        }
        if ((input->group1Input & _BV(dpadRight)) == dpadRightPressed)
        {
            printf("<p>D-Pad right pressed</p>");
        }
        if ((input->group1Input & _BV(dpadLeft)) == dpadLeftPressed)
        {
            printf("<p>D-Pad left pressed</p>");
        }
        if ((input->group1Input & _BV(dpadDown)) == dpadDownPressed)
        {
            printf("<p>D-Pad down pressed</p>");
        }
        if ((input->group1Input & _BV(dpadUp)) == dpadUpPressed)
        {
            printf("<p>D-Pad up pressed</p>");
        }
    }

    if (input->group2Input > 0)
    {
        // At least one of the buttons in this group is pressed
        if ((input->group2Input & _BV(buttonY)) == buttonYPressed)
        {
            printf("<p>Y pressed</p>");
        }
        if ((input->group2Input & _BV(buttonX)) == buttonXPressed)
        {
            printf("<p>X pressed</p>");
        }
        if ((input->group2Input & _BV(buttonB)) == buttonBPressed)
        {
            printf("<p>B pressed</p>");
        }
        if ((input->group2Input & _BV(buttonA)) == buttonAPressed)
        {
            printf("<p>A pressed</p>");
        }
        if ((input->group2Input & _BV(buttonXBOX)) == buttonXBOXPressed)
        {
            printf("<p>XBOX button pressed</p>");
        }
        if ((input->group2Input & _BV(rightShoulder)) == rightShoulderPressed)
        {
            printf("<p>Right shoulder pressed</p>");
        }
        if ((input->group2Input & _BV(leftShoulder)) == leftShoulderPressed)
        {
            printf("<p>Left shoulder pressed</p>");
        }
    }
}

int main(void)
{
    // Starting to create the HTML:
    header("text/html\n");
    startPage("Controller Controller\n");

    char* data;
    long choice;

    printf("<h1>Choose a number:</h1>\n");
    printf("<p>[0]      LEDs Blink </p>"
        "<p>[1]     LEDs Spin</p>"
        "<p>[2]     LEDs Off</p>"
        "<p>[3]     Left Rumbler on</p>"
        "<p>[4]     Left Rumbler off</p>"
        "<p>[5]     Right Rumbler on</p>"
        "<p>[6]     Right Rumbler off</p>"
        "<p>[7]     Read Controller input</p>");
    
    // Search the environment list for QUERY_STRING
    data = getenv("QUERY_STRING");
    
    if (data == NULL)
        printf("<p>No message found</p>\n");
    else if (sscanf(data, "choice=%ld", &choice) != 1)  // Check if there is a variable sent to the cgi
        printf("<p>Invalid data. Data must be numeric.</p>");
    else
    {
        if (choice == 7)
        {
            reading();
            //handleInput(choice);
        }
        else if (choice >= 0 && choice <= 6)
        {
            handleInput(choice);
        }

    }

    endPage();

    return 0;
}