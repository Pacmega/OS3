#
#		Makefile for Semaphores
#
#		© Joran van Teeffelen & Bas Witters
#

BINARIES = ProcesA ProcesB

#CC = i568-linux-g++
CC = g++
CFLAGS = -g3 -Wall -W -Wshadow -Wcast-qual -Wwrite-strings
LDLIBS = -lrt
PROCESA = ProcesA.o 
PROCESB = ProcesB.o

all: $(BINARIES)

clean:
	rm *.o $(BINARIES)