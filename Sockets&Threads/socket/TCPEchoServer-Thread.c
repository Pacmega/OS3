#include <pthread.h>
#include <stdio.h>

#include "Auxiliary.h"
#include "AcceptTCPConnection.h"
#include "CreateTCPServerSocket.h"
#include "HandleTCPClient.h"

static void * myThread (void * arg);            /* thread that does the work */

int main (int argc, char *argv[])
{
    int         servSock;     /* Socket descriptor for server */
    int         clntSock;     /* Socket descriptor for client */
    pthread_t   threadID;     /* Thread ID from pthread_create() */
    bool        to_quit = false;

    parse_args (argc, argv);

    servSock = CreateTCPServerSocket (argv_port);

    while (to_quit == false)                /* run until someone indicates to quit... */
    {
        clntSock = AcceptTCPConnection (servSock);

        void * threadArgs = *((void *)clntSock);

        int threadReturn = pthread_create(&threadID, NULL, myThread, threadArgs);
        if (threadReturn != 0)
        {
            printf("An error occured creating the thread. Error ID: %d \n", threadReturn);
            return(-1);
        }
    }
    
    // server stops...
	return (0);
}

static void *
myThread (void * threadArgs)
{
    int ClntSock = *((int *)threadArgs);
    HandleTCPClient(ClntSock);
    pthread_detach(pthread_self());
    return (NULL);
}
