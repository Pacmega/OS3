#include <libusb-1.0/libusb.h>
#include <stdio.h>
#include "usb.h"

unsigned char inputReport[14] = {0};

void printReport()
{
	printf("Input report\n");
	printf("Message type\t\t%d\n", inputReport[messageType]);
	printf("Packet size (B)\t\t%d\n", inputReport[packetSize]);
	printf("D-Pad + button group 1\t%d\n", inputReport[inputGroup1]);
	printf("(Start/Back & stick press)\n");
	printf("Button group 2\t\t%d\n", inputReport[inputGroup2]);
	printf("(Shoulders, A/B/X/Y, Xbox logo)\n");
	printf("Left trigger\t\t%d\n", inputReport[leftTrigger]);
	printf("Right trigger\t\t%d\n", inputReport[rightTrigger]);
	
	printf("Left stick values\n");
	printf("%d\n", inputReport[6]);
	printf("%d\n", inputReport[7]);
	printf("%d\n", inputReport[8]);
	printf("%d\n", inputReport[9]);
	
	printf("Right stick values\n");
	printf("%d\n", inputReport[10]);
	printf("%d\n", inputReport[11]);
	printf("%d\n", inputReport[12]);
	printf("%d\n", inputReport[13]);
}

void interpretTriggers()
{
	int deadzone = 10;

	if (inputReport[leftTrigger] > deadzone)
	{
		printf("Left trigger pressed\n");
	}
	
	if (inputReport[rightTrigger] > deadzone)
	{
		printf("Right trigger pressed\n");
	}
}

void interpretGroup1()
{
	if (inputReport[inputGroup1] > 0)
	{
		// At least one of the buttons in this group is pressed

		if (inputReport[inputGroup1] >= rightStick)
		{
			inputReport[inputGroup1] -= rightStick;
			printf("Right stick pressed\n");
		}
		if (inputReport[inputGroup1] >= leftStick)
		{
			inputReport[inputGroup1] -= leftStick;
			printf("Left stick pressed\n");
		}
		if (inputReport[inputGroup1] >= backButton)
		{
			inputReport[inputGroup1] -= backButton;
			printf("Back pressed\n");
		}
		if (inputReport[inputGroup1] >= startButton)
		{
			inputReport[inputGroup1] -= startButton;
			printf("Start pressed\n");
		}

		if (inputReport[inputGroup1] >= dpadRight)
		{
			inputReport[inputGroup1] -= dpadRight;
			printf("D-Pad right pressed\n");
		}
		if (inputReport[inputGroup1] >= dpadLeft)
		{
			inputReport[inputGroup1] -= dpadLeft;
			printf("D-Pad left pressed\n");
		}
		if (inputReport[inputGroup1] >= dpadDown)
		{
			inputReport[inputGroup1] -= dpadDown;
			printf("D-Pad down pressed\n");
		}
		if (inputReport[inputGroup1] >= dpadUp)
		{
			inputReport[inputGroup1] -= dpadUp;
			printf("D-Pad up pressed\n");
		}
	}
}

void interpretGroup2()
{
	unsigned char group2Values = inputReport[inputGroup2];
	if (group2Values > 0)
	{
		// At least one of the buttons in this group is pressed
		bool ABXYpressed = false;

		if (group2Values >= buttonA)
		{
			ABXYpressed = true;
		}

		if (group2Values >= buttonY)
		{
			group2Values -= buttonY;
			printf("Y ");
		}
		if (group2Values >= buttonX)
		{
			group2Values -= buttonX;
			printf("X ");
		}
		if (group2Values >= buttonB)
		{
			group2Values-= buttonB;
			printf("B ");
		}
		if (group2Values >= buttonA)
		{
			group2Values -= buttonA;
			printf("A ");
		}
		if (ABXYpressed)
		{
			printf("pressed\n");
		}

		if (group2Values >= buttonXBOX)
		{
			group2Values -= buttonXBOX;
			printf("XBOX button pressed\n");
		}

		if (group2Values >= rightShoulder)
		{
			group2Values -= rightShoulder;
			printf("Right shoulder pressed\n");
		}
		if (group2Values >= leftShoulder)
		{
			group2Values -= leftShoulder;
			printf("Left shoulder pressed\n");
		}
	}
}

void interpretButtons()
{
	interpretGroup1();
	interpretGroup2();
	interpretTriggers();
}

void rumbleSetting(unsigned char& smallRumbler, unsigned char& bigRumbler)
{
	smallRumbler = inputReport[leftTrigger];
	bigRumbler = inputReport[rightTrigger];
}

void lightSetting(unsigned char& lightMode)
{
	if (inputReport[inputGroup2] >= buttonY)
	{
		inputReport[inputGroup2] -= buttonY;
		lightMode = LED1on;
	}
	if (inputReport[inputGroup2] >= buttonX)
	{
		inputReport[inputGroup2] -= buttonX;
		lightMode = LED2on;
	}
	if (inputReport[inputGroup2] >= buttonB)
	{
		inputReport[inputGroup2] -= buttonB;
		lightMode = LED3on;
	}
	if (inputReport[inputGroup2] >= buttonA)
	{
		inputReport[inputGroup2] -= buttonA;
		lightMode = LED4on;
	}
}

int sendNewSettings(libusb_device_handle *device, unsigned char& lightMode, unsigned char& smallRumbler, unsigned char& bigRumbler)
{
	int error, transferred;

	unsigned char lightMessage[] = {1, 3, lightMode};
	error = libusb_interrupt_transfer(device, 0x01, lightMessage, sizeof(lightMessage), &transferred, 0);

	if (error == 0)
	{
		// If that one failed we wouldn't even need to bother with the second one
		unsigned char rumbleMessage[] = {0x00, 0x08, 0x00, bigRumbler, smallRumbler, 0x00, 0x00, 0x00};
		error = libusb_interrupt_transfer(device, 0x01, rumbleMessage, sizeof(rumbleMessage), &transferred, 0);
	}

	return error;
}

int main(int argc, char *argv[])
{
	libusb_device_handle *h;
	
	int error, transferred;
	libusb_init(NULL);
	h = libusb_open_device_with_vid_pid(NULL, 0x045e, 0x028e);

	if (h == NULL)
	{
		fprintf(stderr, "Failed to open device\n");
		return (1);
	}
	error = 0;
	
	unsigned char smallRumbler = 0x00;
	unsigned char bigRumbler = 0x00;

	unsigned char lightMode = 0x00;

	while(true)
	{
		if ((error = libusb_interrupt_transfer(h, 0x81, inputReport, sizeof(inputReport), &transferred, 0)) != 0)
		{
			fprintf(stderr, "Transfer failed: %d\n", error);
			return (1);
		}
		else
		{
			printReport();
			interpretButtons();
			rumbleSetting(smallRumbler, bigRumbler);
			lightSetting(lightMode);

			if ((error = sendNewSettings(h, lightMode, smallRumbler, bigRumbler)) != 0)
			{
				fprintf(stderr, "Transfer failed: %d\n", error);
				return (1);
			}
		}
	}

	return (0);
}
