CFLAGS=-O2 -Wall -Werror

USB_SOURCES=usb/usb.cpp

USB_LIB=-lusb-1.0

GCC=g++

ARM=/home/student/buildroot/output/host/usr/bin/arm-linux-gcc

USBtest: $(USB_SOURCES) Makefile
	$(GCC) $(CFLAGS) $(USB_SOURCES) $(USB_LIB) -o $@

PiTest: $(USB_SOURCES) Makefile
	$(ARM) $(CFLAGS) $(USB_SOURCES) $(USB_LIB) -o $@