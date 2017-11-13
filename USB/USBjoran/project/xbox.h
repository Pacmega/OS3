#ifndef _XBOX_H
#define _XBOX_H

//------------------------------------LEDS
#define LEDsOff 			0x00
#define LEDsBlinking 		0x01

#define LED1Flashing	 	0x02
#define LED2Flashing	 	0x03
#define LED3Flashing	 	0x04
#define LED4Flashing	 	0x05

#define LED1On	 			0x06
#define LED2On	 			0x07
#define LED3On	 			0x08
#define LED4On	 			0x09

#define LEDsRotating 		0x0A
#define LEDsBlinking2 		0x0B
#define LEDsSlowBlinking 	0x0C
#define LEDsAlternating 	0X0D

//------------------------------------INPUT 1

#define DPadUp 		1
#define DPadDown 	2
#define DPadLeft 	4
#define DPadRight	8
#define Start		16
#define Select	 	32
#define LSPressed	64
#define RSPressed	128

//------------------------------------INPUT 2

#define LBumperPressed	1
#define RBumperPressed	2
#define XLogo 			4
#define Apressed		16
#define Bpressed		32
#define Xpressed		64
#define Ypressed		128

#endif
