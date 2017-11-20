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

        int threadReturn = pthread_create(&threadID, NULL, myThread((void *)clntSock), NULL);
        if (threadReturn != 0)
        {
            printf("An error occured creating the thread. Error ID: %d \n", threadReturn);
            return(-1);
        }
        pthread_join(&threadID, NULL);
        // TODO: create&start the thread myThread() te creeeren
        // use the POSIX operation pthread_create()
        //
        // make sure that clntSock and servSock are closed at the correct locations 
        // (in particular: at those places where you don't need them any more)

    }
    
    // server stops...
	return (0);
}

static void *
myThread (void * threadArgs)
{
    HandleTCPClient((int)threadArgs);
    // TODO: write the code to handle the client data
    // use operation HandleTCPClient()
    //  
    // Hint: use the info(), info_d(), info_s() operations to trace what happens
    //
    // Note: a call of pthread_detach() is obligatory
    pthread_detach(pthread_self());
    return (NULL);
}
