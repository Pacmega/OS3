#include <string.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/fcntl.h>

typedef struct
{
    unsigned char group1;       // D-Pad, start and back buttons and stick presses
    unsigned char group2;       // Shoulder buttons, A/B/X/Y and Xbox logo
    unsigned char leftTrigger;  // Left trigger
    unsigned char rightTrigger; // Right trigger
    int           leftStick;    // Left stick
    int           rightStick;   // Right stick
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
