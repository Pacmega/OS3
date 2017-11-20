#include <stdio.h>      // for printf
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
    char        echoBuffer[RCVBUFSIZE]; /* Buffer for received string */
    int         echoStringLen;          /* Length of string to echo */
    char        echoString[RCVBUFSIZE];

    parse_args (argc, argv);

    sock = CreateTCPClientSocket (argv_ip, argv_port);

    if(sock == -1)
    {
        printf("Failed to create a socket. \n");
        return -1;
    }

    while(true)
    {
        printf("You: ");
        scanf("%[^\n]%*c", echoString);
        echoStringLen = strlen(echoString);

        if (send(sock, echoString, echoStringLen, 0) < 0)
        {
            printf("Error sending messages\n");
        }

        if(strcmp(echoString, "Quit") == 0)
        {
            printf("Spotted FBI. Now closing the client. \n");
            break;
        }

        // delaying();
        for (int i = 0; i < RCVBUFSIZE; ++i)
        {
            echoBuffer[i] = '\0';
        }

        if (recv(sock, echoBuffer, RCVBUFSIZE, 0) >= 0)
        {
            printf("Server: %s\n", echoBuffer);
            if (strcmp(echoBuffer, "Quit") == 0)
            {
                printf("Spotted FBI on server-side. Now closing the client. \n");
                break;
            }
        }
    }

    // delaying ();

    close (sock);
    info ("close & exit");
    exit(0);
}
