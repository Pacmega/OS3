DAEMON_APP = deamonApp
CGI_APP = cgiApp

CFLAGS = -O2 -Wall -Werror -lrt -lm -pthread -o

LIBRARIES = $(wildcard lib/*.h)
DAEMON_SOURCE = code/daemon.c
CGI_SOURCE = code/cgi.c
# Not quite sure if that's how CGI is actually going to work

CC=gcc

all: clean daemon cgi

daemon: $(DAEMON_SOURCE)
	$(CC) $(DAEMON_SOURCE) $(LIBRARIES) $(CFLAGS) $(DAEMON_APP)

cgi: $(CGI_SOURCE)
	$(CC) $(CGI_SOURCE) $(LIBRARIES) $(CFLAGS) $(CGI_APP)

clean:
	@rm -rf $(DAEMON_APP)
	@rm -rf $(CGI_APP)