PROJECT_NAME_A = semaphoresA
PROJECT_NAME_B = semaphoresB
PROJECT_NAME_TEST = semaphoresT
PROJECT_FINAL  = semaphores

CFLAGS = -O2 -Wall -Werror -lrt -pthread -o

MAIN_SOURCE_A = ProcessA.c
MAIN_SOURCE_B = ProcessB.c
TEST_SOURCE_W = Wtesting.c
PROJECT = SemaphoreReadWriteProgram.c semshm.c

GCC = gcc

all: sourceA sourceB

rw: project

sourceA: $(MAIN_SOURCE_A) 
	$(GCC) $(MAIN_SOURCE_A) $(CFLAGS) $(PROJECT_NAME_A)

sourceB: $(MAIN_SOURCE_B) 
	$(GCC) $(MAIN_SOURCE_B) $(CFLAGS) $(PROJECT_NAME_B)

project: $(PROJECT) 
	$(GCC) $(PROJECT) $(CFLAGS) $(PROJECT_FINAL)

test: $(TEST_SOURCE_W) 
	$(GCC) $(TEST_SOURCE_W) $(CFLAGS) $(PROJECT_NAME_TEST)

clean:
	@rm -rf $(PROJECT_NAME_A)
	@rm -rf $(PROJECT_NAME_B)
	@rm -rf $(PROJECT_NAME_TEST)
	@rm -rf $(PROJECT_FINAL)