DAEMON_APP = deamonApp
CGI_APP = cgiApp

CFLAGS = -O2 -Wall -Werror -lrt -pthread -o

LIBRARIES = $(wildcard lib/*.c)
DAEMON_SOURCE = code/daemon.c
CGI_SOURCE = code/cgi.c
# Not quite sure if that's how CGI is actually going to work

CC=gcc

all: clean deamon cgi

daemon: $(DAEMON_SOURCE)
	$(GCC) $(DAEMON_SOURCE) $(CFLAGS) $(LIBRARIES) $(DAEMON_APP)

cgi: $(CGI_SOURCE)
	$(GCC) $(CGI_SOURCE) $(CFLAGS) $(LIBRARIES) $(CGI_APP)

clean:
	@rm -rf *.o