PROJECT_NAME_A = semaphoresA
PROJECT_NAME_B = semaphoresB
PROJECT_NAME_TEST = semaphoresT
PROJECT_FINAL  = semaphores
PROJECT_FORK   = semaphoresF
PROJECT_READ   = semaphoresR
PROJECT_WRITE  = semaphoresW

CFLAGS = -O2 -Wall -Werror -lrt -pthread -o

MAIN_SOURCE_A = examples/ProcessA.c
MAIN_SOURCE_B = examples/ProcessB.c
TEST_SOURCE_W = testing/Wtesting.c
PROJECT = SemaphoreReadWriteProgram.c semshm.c
FORK = ForkaphoreReadWriteProgram.c semshm.c
READ = readProcess.c semshm.c
WRITE = writeProcess.c semshm.c

GCC = gcc

all: sourceA sourceB

sourceA: $(MAIN_SOURCE_A) 
	$(GCC) $(MAIN_SOURCE_A) $(CFLAGS) $(PROJECT_NAME_A)

sourceB: $(MAIN_SOURCE_B) 
	$(GCC) $(MAIN_SOURCE_B) $(CFLAGS) $(PROJECT_NAME_B)

project: $(PROJECT) 
	$(GCC) $(PROJECT) $(CFLAGS) $(PROJECT_FINAL)

fork: $(FORK)
	$(GCC) $(FORK) $(CFLAGS) $(PROJECT_FORK)

test: $(TEST_SOURCE_W) 
	$(GCC) $(TEST_SOURCE_W) $(CFLAGS) $(PROJECT_NAME_TEST)

rw: read write

read: $(READ)
	$(GCC) $(READ) $(CFLAGS) $(PROJECT_READ)

write: $(WRITE)
	$(GCC) $(WRITE) $(CFLAGS) $(PROJECT_WRITE)

clean:
	@rm -rf $(PROJECT_NAME_A)
	@rm -rf $(PROJECT_NAME_B)
	@rm -rf $(PROJECT_NAME_TEST)
	@rm -rf $(PROJECT_FINAL)
	@rm -rf $(PROJECT_FORK)
	@rm -rf $(PROJECT_READ)
	@rm -rf $(PROJECT_WRITE)