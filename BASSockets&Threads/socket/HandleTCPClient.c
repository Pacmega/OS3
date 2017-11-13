#include <stdio.h>
#include <ctype.h>      // for isupper() etc.
#include <sys/socket.h> // for send() and recv()
#include <unistd.h>     // for sleep(), close()

#include "Auxiliary.h"
#include "HandleTCPClient.h"

#define RCVBUFSIZE 32   /* Size of receive buffer */

void HandleTCPClient (int clntSocket)
{
    // 'clntSocket' is obtained from AcceptTCPConnection()

    char echoBuffer[RCVBUFSIZE];        /* Buffer for echo string */
    int  recvMsgSize;                   /* Size of received message */

    /* Receive message from client */
    recvMsgSize = recv (clntSocket, echoBuffer, RCVBUFSIZE-2, 0);
    
    if (recvMsgSize < 0)
    {
        DieWithError ("recv() failed");
    }
    
    info_d ("Recv", recvMsgSize);

    /* Send received string and receive again until end of transmission */
    while (recvMsgSize > 0)      /* zero indicates end of transmission */
    {
        printf("Received string: %s\n", echoBuffer);
        
        for (int b = 0; b < recvMsgSize; b++)
        {
            if (islower(echoBuffer[b]))
            {
                echoBuffer[b] = toupper(echoBuffer[b]);
            }
            else if(isupper(echoBuffer[b]))
            {
                echoBuffer[b] = tolower(echoBuffer[b]);
            }
        }
        
        delaying ();
        
        /* Echo message back to client */
        if (send (clntSocket, echoBuffer, recvMsgSize, 0) != recvMsgSize)
        {
            DieWithError ("send() failed");
        }

        info_s("Verbose mode", echoBuffer);

        for(int f = 0; f < RCVBUFSIZE; f++)
        {
            echoBuffer[f] = '\0';
        }

        // receive next string
        recvMsgSize = recv (clntSocket, echoBuffer, RCVBUFSIZE-1, 0);
        if (recvMsgSize < 0)
        {
            DieWithError ("recv() failed");
        }
        info_d ("recv", recvMsgSize);
    }

    close (clntSocket);    /* Close client socket */
    info ("close");
}
