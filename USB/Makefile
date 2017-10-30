CFLAGS=-O2 -Wall -Werror

TEST_SOURCES=test/test.cpp

USB_LIB=-lusb-1.0

GCC=g++

all: testDing

main: $

USBtest: $(TEST_SOURCES) Makefile
	$(GCC) $(CFLAGS) $(TEST_SOURCES) $(USB_LIB) -o $@