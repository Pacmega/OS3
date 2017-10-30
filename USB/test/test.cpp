/*
* de volgende code laat het ledje op de gamepad rondspinnen:
*/

#include <libusb-1.0/libusb.h>
#include <stdio.h>
#include <iostream>

int main(int argc, char *argv[])
{
	libusb_device_handle *h;
	unsigned char data[] = { 1, 3, 0x0a };
	unsigned char data2[] = { 1, 3, 0x05 };
	int error, transferred;
	libusb_init(NULL);
	h = libusb_open_device_with_vid_pid(NULL, 0x045e, 0x028e);

	if (h == NULL)
	{
		fprintf(stderr, "Failed to open device\n");
		return (1);
	}
	error = 0;
	
	std::cout << error << std::endl;

	if ((error = libusb_interrupt_transfer(h, 0x01, data, sizeof(data), &transferred, 0)) != 0)
	{
		fprintf(stderr, "Transfer failed: %d\n", error);
		return (1);
	}
	
	if ((error = libusb_interrupt_transfer(h, 0x01, data2, sizeof(data), &transferred, 0)) != 0)
	{
		fprintf(stderr, "Transfer failed: %d\n", error);
		return (1);
	}

	std::cout << transferred << std::endl;

	return (0);
}