#include <string.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/fcntl.h>

#define PRONUNCIATIONLENGTH 7

typedef struct
{
	int 	value;
	char 	pronunciation[PRONUNCIATIONLENGTH];
} number;

typedef struct
{
	char * 	sharedMem;
	sem_t * itemsFilled;
	sem_t * spaceLeft;
} multithreading;