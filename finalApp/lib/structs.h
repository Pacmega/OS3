#include <string.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/fcntl.h>

typedef struct
{
	int 	value;
	char* 	pronunciation;
} number;

typedef struct
{
	char * 	sharedMem;
	sem_t * itemsFilled;
	sem_t * spaceLeft;
} multithreading;