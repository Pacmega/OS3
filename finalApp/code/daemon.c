#include <stdio.h>      // Printing to the terminal
#include <string.h>     // Strings
#include <sys/types.h>  // Shared memory
#include <sys/mman.h>   // Shared memory
#include <sys/stat.h>   // Shared memory
#include <sys/fcntl.h>  // Shared memory
#include <semaphore.h>  // For POSIX semaphores
#include <pthread.h>    // For POSIX threads
#include <mqueue.h>     // For POSIX message queues
#include <stdbool.h>    // Booleans
#include <unistd.h>     // Sleep()
#include <signal.h>     // For the keyboard interrupt

// TODO: create a more useful structs file if it's even needed
#include "../lib/structs.h"    // Data about the multithreading and number structs

// TODO: go through this file and clean it up (and understand it)
// TODO: rename this file
#include "../lib/auxiliary.h"  // TODO: describe what this header is useful for

#include "../lib/semshm.h"     // Semaphore & shared memory management functions
#include "../lib/xboxUSB.h"    // Xbox 360 USB information and _BV() macro

int main(int argc, char const *argv[])
{
    printf("This daemon is useless. Actually, it's not even a daemon right now.\n");
    printf("1 << 4 = %d\n", _BV(4));
    return 0;
}