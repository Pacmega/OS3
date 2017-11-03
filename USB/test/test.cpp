#include <libusb-1.0/libusb.h>
#include <stdio.h>

void Lighting(libusb_device_handle *h, int selector);
void Motors(libusb_device_handle *h, unsigned char x, unsigned char y);
void Row2(int selector);
void ReadLeftStick(unsigned char x, unsigned char y);
void ReadRightStick(unsigned char x, unsigned char y);
void ClearScreen();

int error, transferred;

int main(int argc, char *argv[])
{
	libusb_device_handle *h;
	unsigned char inputData[20];				  //	//

	libusb_init(NULL);
	h = libusb_open_device_with_vid_pid(NULL, 0x045e, 0x028e);

	if (h == NULL)
	{
		fprintf(stderr, "Failed to open device\n");
		return (1);
	}

	while(true)
	{
		if ((error = libusb_interrupt_transfer(h, 0x81, inputData, sizeof(inputData), &transferred, 0)) = 0)
		{
			fprintf(stderr, "Failed to retrieve buttondata\n");
		}
		ClearScreen();
		Row2(inputData[2]);
		Lighting(h, inputData[3]);
		Motors(h, inputData[4], inputData[5]);
		ReadLeftStick(inputData[6], inputData[8]);
		ReadRightStick(inputData[10], inputData[12]);
	}

	return (0);
}

void Motors(libusb_device_handle *h, unsigned char x, unsigned char y)
{
	if (x == 0xFF)
	{
		fprintf(stderr, "Left Trigger fully pressed \n");
	}
	if (y == 0xFF)
	{
		fprintf(stderr, "Right Trigger fully pressed\n");
	}
	unsigned char data2[] = { 0x00, 0x08, 0x00, x, y, 0x00, 0x00, 0x00 };

	if ((error = libusb_interrupt_transfer(h, 0x01, data2, sizeof(data2), &transferred, 0)) != 0)
	{
		fprintf(stderr, "Vibrationtransfer failed: %d\n", error);
	}
}

void Lighting(libusb_device_handle *h, int selector)
{
	unsigned char effect = 0x00;

		if(selector == 1)
		{
			fprintf(stderr, "Left Bumper pressed \n");
			effect = 0x01;
		}
		if(selector == 2)
		{
			fprintf(stderr, "Right Bumper pressed \n");
			effect = 0x0C;
		}		
		if(selector == 4)
		{
			fprintf(stderr, "X-logo pressed \n");
			effect = 0x0A;
		}		
		if(selector == 16)
		{
			fprintf(stderr, "A pressed \n");
			effect = 0x06;
		}		
		if(selector == 32)
		{
			fprintf(stderr, "B pressed \n");
			effect = 0x07;
		}		
		if(selector == 64)
		{
			fprintf(stderr, "X pressed \n");
			effect = 0x08;
		}		
		if(selector == 128)
		{
			fprintf(stderr, "Y pressed \n");
			effect = 0x09;
		}

	unsigned char data[] = { 1, 3, effect };

	if ((error = libusb_interrupt_transfer(h, 0x01, data, sizeof(data), &transferred, 0)) != 0)
	{
		fprintf(stderr, "Transfer failed: %d\n", error);
	}
}

void Row2(int selector)
{

	if(selector == 1)
		fprintf(stderr, "D-pad up pressed \n");

	if(selector == 2)
		fprintf(stderr, "D-pad down pressed \n");

	if(selector == 4)
		fprintf(stderr, "D-pad left pressed \n");
	
	if(selector == 8)
		fprintf(stderr, "D-pad right pressed\n");

	if(selector == 16)
		fprintf(stderr, "Start pressed \n");

	if(selector == 32)
		fprintf(stderr, "Select pressed \n");

	if(selector == 64)
		fprintf(stderr, "Left stick pressed \n");
	
	if(selector == 128)
		fprintf(stderr, "Right stick pressed \n");

}

void ReadLeftStick(unsigned char x, unsigned char y)
{
	if(x == 0)
		fprintf(stderr, "Left stick left\n");

	if(x > 250)
		fprintf(stderr, "Left Stick right\n");

	if(y == 0)
		fprintf(stderr, "Left Stick down\n");

	if(y > 250)
		fprintf(stderr, "Left Stick up\n");
}

void ReadRightStick(unsigned char x, unsigned char y)
{
	if(x == 0)
		fprintf(stderr, "Right stick left\n");

	if(x > 250)
		fprintf(stderr, "Right Stick right\n");

	if(y == 0)
		fprintf(stderr, "Right Stick down\n");

	if(y > 250)
		fprintf(stderr, "Right Stick up\n");
}

void ClearScreen()
{
	int n;
    for (n = 0; n < 10; n++)
      printf( "\n\n\n\n\n\n\n\n\n\n");
}