#include <string.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/fcntl.h>
#include <libusb-1.0/libusb.h>
#include <mqueue.h>

typedef struct
{
    unsigned char group1Input;       // D-Pad, start and back buttons and stick presses
    unsigned char group2Input;       // Shoulder buttons, A/B/X/Y and Xbox logo
    unsigned char leftTriggerInput;  // Left trigger
    unsigned char rightTriggerInput; // Right trigger
    int           leftStickInput;    // Left stick
    int           rightStickInput;   // Right stick
} inputStruct;

typedef struct
{
	char * 	sharedMemory;
	sem_t * itemAvailableSem;
	sem_t * itemRequestedSem;
	libusb_device_handle * deviceHandle;
	mqd_t messageQueue;
} multithreading;