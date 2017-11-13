#include <stdio.h>
#include <ctype.h>      // for isupper() etc.
#include <sys/socket.h> // for send() and recv()
#include <unistd.h>     // for sleep(), close()
#include <string.h>     // for strlen()

#include "Auxiliary.h"
#include "HandleTCPClient.h"
#include "CreateTCPServerSocket.h"
#include "AcceptTCPConnection.h"

#define RCVBUFSIZE 32   /* Size of receive buffer */

int main (int argc, char * argv[])
{
    // 'clntSocket' is obtained from AcceptTCPConnection()
    char        echoBuffer[RCVBUFSIZE]; /* Buffer for received string */
    int         echoStringLen;          /* Length of string to echo */
    int         clntSock; 
    int         servSock;
    int         recvMsgSize;            // Size of received message

    parse_args(argc, argv);

    servSock = CreateTCPServerSocket(argv_port);
    clntSock = AcceptTCPConnection(servSock);

    while(true)
    {
        char echoString[RCVBUFSIZE];

        for (int i = 0; i < RCVBUFSIZE; ++i)
        {
            echoBuffer[i] = '\0';
        }

        if (recv(clntSock, echoBuffer, RCVBUFSIZE, 0) >= 0)
        {
            printf("Client: ");
            printf("%s\n", echoBuffer);
        }

        else
        {
            printf("Error receiving message \n");
        }

        scanf("%[^ \n]%*c", echoString);
        echoStringLen = strlen(echoString);
        if (strcmp(echoString, "Quit") == 0)
        {
            printf("Spotted FBI. Now closing the server. \n");
            break;
        }

        if (send(clntSock, echoString, echoStringLen, 0) == -1)
        {
            printf("Error sending message.");
        }

    }
    close (clntSock);    /* Close client socket */
    exit(0);
}
