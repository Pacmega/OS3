#include <string.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/fcntl.h>

typedef struct
{
    unsigned char group1Input;       // D-Pad, start and back buttons and stick presses
    unsigned char group2Input;       // Shoulder buttons, A/B/X/Y and Xbox logo
    unsigned char leftTriggerInput;  // Left trigger
    unsigned char rightTriggerInput; // Right trigger
    int           leftStickInput;    // Left stick
    int           rightStickInput;   // Right stick
    /*
    printf("Input report\n");
    printf("Message type\t\t%d\n", inputReport[messageType]);
    printf("Packet size (B)\t\t%d\n", inputReport[packetSize]);
    printf("D-Pad + button group 1\t%d\n", inputReport[inputGroup1]);
    printf("(Start/Back & stick press)\n");
    printf("Button group 2\t\t%d\n", inputReport[inputGroup2]);
    printf("(Shoulders, A/B/X/Y, Xbox logo)\n");
    printf("Left trigger\t\t%d\n", inputReport[leftTrigger]);
    printf("Right trigger\t\t%d\n", inputReport[rightTrigger]);
    */
} inputStruct;

typedef struct 
{
	int 	leftRumbler;
	int		rightRumbler;
	int		lightFunction;
}x360outputs;

typedef struct
{
	
}x360inputs;
