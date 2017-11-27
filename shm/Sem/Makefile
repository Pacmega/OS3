PROJECT_NAME_A = semaphoresA
PROJECT_NAME_B = semaphoresB
PROJECT_NAME_W = semaphoresW

# Ignored CFLAGS -Werror
CFLAGS = -O2 -Wall -lrt -pthread -o

MAIN_SOURCE_A = ProcessA.c
MAIN_SOURCE_B = ProcessB.c
TEST_SOURCE_W = Wtesting.c

GCC = gcc

all: sourceA sourceB

test: Wtest

sourceA: $(MAIN_SOURCE_A) 
	$(GCC) $(MAIN_SOURCE_A) $(CFLAGS) $(PROJECT_NAME_A)

sourceB: $(MAIN_SOURCE_B) 
	$(GCC) $(MAIN_SOURCE_B) $(CFLAGS) $(PROJECT_NAME_B)

Wtest: $(TEST_SOURCE_W) 
	$(GCC) $(TEST_SOURCE_W) $(CFLAGS) $(PROJECT_NAME_W)

clean:
	@rm -rf $(PROJECT_NAME_A)
	@rm -rf $(PROJECT_NAME_B)
	@rm -rf $(PROJECT_NAME_W)