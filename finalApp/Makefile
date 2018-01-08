DAEMON_APP = deamonApp
CGI_APP = cgiApp

CFLAGS = -O2 -Wall -Werror -lrt -lm -pthread -o

LIBUSB = -lusb-1.0
CGI_LIBRARIES = lib/semshm.c
DAEMON_SOURCE = code/daemon.c
CGI_SOURCE = code/cgi.c
# TODO: Not quite sure if that's how CGI is actually going to work

CC=gcc

all: clean daemon cgi

daemon: $(DAEMON_SOURCE)
	$(CC) $(DAEMON_SOURCE) $(LIBRARIES) $(LIBUSB) $(CFLAGS) $(DAEMON_APP)

cgi: $(CGI_SOURCE)
	$(CC) $(CGI_SOURCE) $(CGI_LIBRARIES) $(CFLAGS) $(CGI_APP)

clean:
	@rm -rf $(DAEMON_APP)
	@rm -rf $(CGI_APP)