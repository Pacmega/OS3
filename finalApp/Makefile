DAEMON_APP = deamonApp
CGI_APP = cgiApp

CFLAGS = -O2 -Wall -Werror -lrt -lm -pthread -o

# UNUSED_LIBRARIES_COMMAND = $(wildcard lib/*.h)
CGI_LIBRARIES = lib/semshm.c
DAEMON_SOURCE = code/daemon.c
CGI_SOURCE = code/cgi.c
# Not quite sure if that's how CGI is actually going to work

CC=gcc

all: clean daemon cgi

daemon: $(DAEMON_SOURCE)
	$(CC) $(DAEMON_SOURCE) $(LIBRARIES) $(CFLAGS) $(DAEMON_APP)

cgi: $(CGI_SOURCE)
	$(CC) $(CGI_SOURCE) $(CGI_LIBRARIES) $(CFLAGS) $(CGI_APP)

clean:
	@rm -rf $(DAEMON_APP)
	@rm -rf $(CGI_APP)