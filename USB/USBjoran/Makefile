CFLAGS=-O2 -Wall -Werror

SOURCES=project/main.cpp

USB_LIB=-lusb-1.0

GCC=g++

ARM=../../../../../buildroot/output/host/usr/bin/arm-linux-gcc
VALTNIETOP=../../finalApp/pi2Compilers/arm-linux-gcc

USBtest: $(TEST_SOURCES) Makefile
	$(GCC) $(CFLAGS) $(SOURCES) $(USB_LIB) -o $@

pi: $(TEST_SOURCES) Makefile
	$(ARM) $(CFLAGS) $(SOURCES) $(USB_LIB) -o $@

piB: $(TEST_SOURCES) Makefile
	$(VALTNIETOP) $(CFLAGS) $(SOURCES) $(USB_LIB) -o $@
