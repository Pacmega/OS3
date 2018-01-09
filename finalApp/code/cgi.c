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


void handleInput(int choice)
{
    char mq_name[nameSize] = messageQueueName;
    int rtnval = -1;

    mqd_t mq_fd = -1; 

    mq_fd = mq_open(mq_name, O_WRONLY); // Try to open an existing MQ (Write only mode)

    if (mq_fd != -1)    // if the queue opened
    {
        // Send the outputs to the queue
        rtnval = mq_send(mq_fd, (char *) &choice, sizeof(int), 0);
        if (rtnval == -1)
            printf("Error sending message to queue.\n");

        mq_unlink(mq_name);
    }
    // if the queue didn't open, it means the daemon isnt running correctly
}

void reading()
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

    int rtnval;
    int positionToRead;
    //inputStruct readInput;

    rtnval = sem_wait(availableSem);
    if(rtnval != 0)
    {
        perror("ERROR: sem_wait() failed");
        return;
    }

    // Get a position to read to where there is space available
    rtnval = sem_getvalue(availableSem, &positionToRead);
    if(rtnval != 0)
    {
        perror("ERROR: sem_getvalue() failed");
        return;
    }

    // Get the address of the shared memory where the inputstruct resides
    char* address = my_shm_open(sizeof(inputStruct), memName);

    inputStruct* shm_inputs = (inputStruct*)address;

    rtnval = sem_post(requestSem);
    if(rtnval != 0)
    {
        perror("ERROR: sem_post() failed");
        return;
    }

    printInput(shm_inputs);

    shmCleanup(memory);
    semCleanup(requestName);
    semCleanup(availableName);

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

    // Starting to create the HTML:
    header("text/html\n");
    startPage("Controller Controller\n");

    char* data;
    long choice;

    printf("<p>Choose a number:</p>\n");
    printf("<p>[0]      LEDs Blink </p>"
        "<p>[1]     LEDs Spin</p>"
        "<p>[2]     LEDs Off</p>"
        "<p>[3]     Left Rumbler on</p>"
        "<p>[4]     Right Rumbler on</p>"
        "<p>[5]     Left Rumbler off</p>"
        "<p>[6]     Right Rumbler off</p>"
        "<p>[7]     Read Controller input</p>");
    
    data = getenv("QUERY_STRING");
    
    if (data == NULL)
        printf("No message found\n");
    else if (sscanf(data, "choice=%ld", &choice) != 1)  // Check if there are 3 variables sent to the cgi
        printf("Invalid data. Data must be numeric.\n");
    else
    {
        if (choice == 7)
        {
            reading();
            handleInput(choice);
        }
        else
        {
            handleInput(choice);
        }

    }

    endPage();

    return 0;
}   
