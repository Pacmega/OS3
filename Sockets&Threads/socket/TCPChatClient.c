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
    char        echoBuffer[RCVBUFSIZE]; /* Buffer for received string */
    int         echoStringLen;          /* Length of string to echo */
    int         bytesRcvd;              /* Bytes read in single recv() */
    int         i;                      /* counter for data-arguments */

    parse_args (argc, argv);

    sock = CreateTCPClientSocket (argv_ip, argv_port);

    while(true)
    {
        char echoString[RCVBUFSIZE];
        scanf("%[^ \n]%*c", echoString);
        echoStringLen = strlen(echoString);

        if (strcmp(echoString, "Quit") == 0)
        {
            printf("Spotted FBI. Now closing the client. \n");
            break;
        }

        if (send(sock, echoString, echoStringLen, 0) == -1)
        {
            printf("Error sending message.");
        }

        delaying();
        for (int i = 0; i < RCVBUFSIZE; ++i)
        {
            echoBuffer[i] = '\0';
        }
        if (recv(sock, echoBuffer, RCVBUFSIZE, 0) >= 0)
        {
            printf("Server: ");
            printf("%s\n", echoBuffer);
        }
        else
        {
            printf("Error receiving message \n");
        }
    }

    delaying ();

    close (sock);
    info ("close & exit");
    exit (0);
}
