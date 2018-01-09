PROJECT_READ   = semaphoresR
PROJECT_WRITE  = semaphoresW
PROJECT_THREAD = semaphoresT
PROJECT_FORK   = semaphoresF

CFLAGS = -O2 -Wall -Werror -lrt -pthread -o

READ   = readProcess.c                semshm.c
WRITE  = writeProcess.c               semshm.c
THREAD = SemaphoreReadWriteProgram.c  semshm.c
FORK   = ForkaphoreReadWriteProgram.c semshm.c

GCC = gcc

all: read write thread fork

rw: read write

read: $(READ)
	$(GCC) $(READ) $(CFLAGS) $(PROJECT_READ)

write: $(WRITE)
	$(GCC) $(WRITE) $(CFLAGS) $(PROJECT_WRITE)

thread: $(THREAD) 
	$(GCC) $(THREAD) $(CFLAGS) $(PROJECT_THREAD)

fork: $(FORK)
	$(GCC) $(FORK) $(CFLAGS) $(PROJECT_FORK)

clean:
	@rm -rf $(PROJECT_READ)
	@rm -rf $(PROJECT_WRITE)
	@rm -rf $(PROJECT_THREAD)
	@rm -rf $(PROJECT_FORK)