CFLAGS=-O2 -Wall -Werror

TEST_SOURCES=test/test.cpp

USB_LIB=-lusb-1.0

GCC=g++

ARM=/home/student/buildroot/output/host/usr/bin/arm-linux-gcc

all: testDing

main: $

USBtest: $(TEST_SOURCES) Makefile
	$(GCC) $(CFLAGS) $(TEST_SOURCES) $(USB_LIB) -o $@

PiTest: $(TEST_SOURCES) Makefile
	$(ARM) $(CFLAGS) $(TEST_SOURCES) $(USB_LIB) -o $@