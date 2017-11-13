#include <stdio.h>
#include <string.h>     // for strlen()
#include <stdlib.h>     // for exit()
#include <sys/socket.h> // for send() and recv()
#include <unistd.h>     // for sleep(), close()

#include "Auxiliary.h"
#include "CreateTCPClientSocket.h"

#define RCVBUFSIZE 32   /* Size of receive buffer */

int main (int argc, char *argv[])
{
    int         sock;                   /* Socket descriptor */
    char        userInput[RCVBUFSIZE];   /* String to send to echo server */
    char        messageBuffer[RCVBUFSIZE]; /* Buffer for received string */
    int         userInputLen;          /* Length of string to echo */
    int         bytesRcvd;              /* Bytes read in single recv() */

    parse_args (argc, argv);

    sock = CreateTCPClientSocket (argv_ip, argv_port);
    
    if (sock != -1)
    {
        printf("Socket created.\n");

        while (true)
        {
            printf("Enter a message to send.\n");
            scanf ("%[^\n]%*c", userInput);

            if (strcmp(userInput, "Quit") == 0)
            {
                break;
            }

            userInputLen = strlen (userInput); /* Determine input length */

            if (send(sock, userInput, userInputLen, 0) < 0)
            {
                printf("Send failed.\n");
            }
            else
            {
                printf("Message sent.\nWaiting for reply...\n");
            }
            
            for(int f = 0; f < RCVBUFSIZE; f++)
            {
                messageBuffer[f] = '\0';
            }

            bytesRcvd = recv(sock, messageBuffer, RCVBUFSIZE, 0);

            if (bytesRcvd < 0)
            {
                printf("Receive failed.\n");
            }
            else if (bytesRcvd == 0)
            {
                printf("Connection terminated by server.\n");
                break;
            }
            else
            {
                printf("Message received: \n%s\n", messageBuffer);
            }
        }

        close (sock);
        info ("close & exit");
        exit (0);
    }    
    else
    {
        printf("Socket could not be created.\n");
    }
}
