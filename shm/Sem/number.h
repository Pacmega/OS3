#include <string.h>
#include <semaphore.h>

typedef struct
{
	int value;
	char* pronunciation;
	sem_t * structSem;
} number;