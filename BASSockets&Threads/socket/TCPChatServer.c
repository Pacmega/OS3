#include <stdio.h>
#include <string.h>     // for strlen()
#include <stdlib.h>     // for exit()
#include <sys/socket.h> // for send() and recv()
#include <unistd.h>     // for sleep(), close()

#include "Auxiliary.h"
#include "AcceptTCPConnection.h"
#include "CreateTCPServerSocket.h"
#include "HandleTCPClient.h"

#define RCVBUFSIZE 32   /* Size of receive buffer */

int main (int argc, char * argv[])
{
    int servSock;                    /* Socket descriptor for server */
    int clntSock;                    /* Socket descriptor for client */

    char *      userInput;             /* String to send to echo server */
    char        messageBuffer[RCVBUFSIZE]; /* Buffer for received string */
    int         userInputLen;          /* Length of string to echo */

    parse_args (argc, argv);
    
    servSock = CreateTCPServerSocket (argv_port);

    for (;;) /* Run forever */
    {
        clntSock = AcceptTCPConnection (servSock);

        printf("Waiting for the first message...\n");

        while (true)
        {
            for(int f = 0; f < RCVBUFSIZE; f++)
            {
                messageBuffer[f] = '\0';
            }

            if (recv(clntSock, messageBuffer, RCVBUFSIZE, 0) < 0)
            {
                printf("Receive failed.\n");
            }
            else
            {
                printf("Message received: \n%s\n", messageBuffer);
            }

            printf("Enter a message to send.\n");
            scanf ("%[^\n]%*c", userInput);

            if (strcmp(userInput, "Quit") == 0)
            {
                break;
            }

            userInputLen = strlen (userInput);          /* Determine input length */

            if (send(clntSock, userInput, userInputLen, 0) < 0)
            {
                printf("Send failed.\n");
            }
            else
            {
                printf("Message sent.\nWaiting for reply...\n");
            }
        }

        close (clntSock);    /* Close client socket */
        info ("close");
        exit (0);
    }
    /* NOT REACHED */
}
