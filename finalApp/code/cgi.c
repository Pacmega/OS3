#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/fcntl.h>
#include <mqueue.h>
#include "../lib/structs.h"

int main(void)
{
	char* data;
	long lr, rr, l;		// The values for the left- and right rumbler and light

	data = getenv("QUERY_STRING");
	if (data == NULL)
		printf("Error in passing data from form to script.\n");
	else if (sscanf(data, "lr=%ld&rr=%ld&l%ld", &lr, &rr, &l) != 3)	// Check if there are 3 variables sent to the cgi
		printf("Invalid data. Data must be numeric.\n");
	else
	{
		char mq_name[80] = "/_mq_mq_";
		int rtnval = -1;

		mqd_t mq_fd = -1;
		struct mq_attr attr;

		x360output outputs;
		outputs.leftRumbler = lr;
		outputs.rightRumbler = rr;
		outputs.lightFunction = l;

		mq_fd = mq_open(mq_name, O_WRONLY);	// Try to open an existing MQ (Write only mode)

		if (mq_fd != -1)	// If it exists
		{
			// Send the outputs to the queue
			rtnval = mq_send(mq_fd, (char *) &outputs, sizeof(outputs), 0);
			if (rtnval == -1)
				printf("Error sending message to queue.\n");

		}
		else // Else: open a new MQ
		{
			attr.mq_maxmsg = 3;	// We only need 1 struct at the time, but this is for convenience 
			attr.mq_msgsize = sizeof(x360outputs);
			
			rtnval = mq_fd = mq_open(mq_name, O_WRONLY | O_CREAT | O_EXCL, 0600, &attr);
			if (rtnval == -1)
			{
				printf("Error opening a new queue.\n");
				return -1;
			}

			rtnval = mq_send(mq_fd, (char *) &outputs, sizeof(outputs), 0);

			if (rtnval == -1)
				printf("Error sending message to queue.\n");
		}

		rtnval = mq_close(mq_fd);

		if (rtnval == -1)
			printf("Error closing the queue.\n");	
	}
}
