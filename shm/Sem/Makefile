PROJECT_NAME_A = semaphoresA
PROJECT_NAME_B = semaphoresB

# Ignored CFLAGS -Werror
CFLAGS = -O2 -Wall -lrt -pthread -o

MAIN_SOURCE_A = ProcessA.c
MAIN_SOURCE_B = ProcessB.c

GCC = gcc

all: a b

a: $(MAIN_SOURCE_A) 
	$(GCC) $(MAIN_SOURCE_A) $(CFLAGS) $(PROJECT_NAME_A)

b: $(MAIN_SOURCE_B) 
	$(GCC) $(MAIN_SOURCE_B) $(CFLAGS) $(PROJECT_NAME_B)

clean:
	@rm -rf $(PROJECT_NAME_A)
	@rm -rf $(PROJECT_NAME_B)