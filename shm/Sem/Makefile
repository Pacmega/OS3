#
#		Makefile for Semaphores
#
#		© Joran van Teeffelen & Bas Witters
#

BINARIES = ProcesA ProcesB

#CC = i568-linux-g++
CC = g++
CFLAGS = -O2 -Wall -Werror -lrt
PROCESA = ProcesA.o 
PROCESB = ProcesB.o

all: $(BINARIES)
	$(CFLAGS)

clean:
	rm *.o $(BINARIES)
