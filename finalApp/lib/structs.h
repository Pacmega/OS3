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
} inputStruct;