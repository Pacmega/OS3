DAEMON_APP = deamonApp
CGI_APP = cgiApp

CFLAGS = -O2 -Wall -Werror -lrt -lm -pthread -o

LIBUSB = -lusb-1.0
LIBRARIES = lib/semshm.c
DAEMON_SOURCE = code/daemon.c
CGI_SOURCE = code/cgi.c
# Not quite sure if that's how CGI is actually going to work

CC=gcc
ARM=arm-linux-gcc

all: clean daemon cgi

pi: clean pidaemon picgi

daemon: $(DAEMON_SOURCE)
	$(CC) $(DAEMON_SOURCE) $(LIBRARIES) $(LIBUSB) $(CFLAGS) $(DAEMON_APP)

cgi: $(CGI_SOURCE)
	$(CC) $(CGI_SOURCE) $(LIBRARIES) $(CFLAGS) $(CGI_APP)

pidaemon: $(DAEMON_SOURCE)
	$(ARM) $(DAEMON_SOURCE) $(LIBRARIES) $(CFLAGS) $(DAEMON_APP)

picgi: $(CGI_SOURCE)
	$(ARM) $(CGI_SOURCE) $(CGI_LIBRARIES) $(CFLAGS) $(CGI_APP)

clean:
	@rm -rf $(DAEMON_APP)
	@rm -rf $(CGI_APP)
