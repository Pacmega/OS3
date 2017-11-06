#ifndef _USB_H_
#define _USB_H_

	#define messageType 0
	#define packetSize 1

	// Group 1
	#define inputGroup1 2

	#define rightStick 128
	#define leftStick 64
	#define backButton 32
	#define startButton 16

	#define dpadRight 8
	#define dpadLeft 4
	#define dpadDown 2
	#define dpadUp 1

	// Group 2
	#define inputGroup2 3

	#define buttonY 128
	#define buttonX 64
	#define buttonB 32
	#define buttonA 16

	#define buttonXBOX 4
	#define rightShoulder 2
	#define leftShoulder 1


	#define leftTrigger 4
	#define rightTrigger 5

	// LED modes
	#define allLEDsOff 0x00
	#define allLEDsBlinking 0x01
	#define allLEDsFlashOnce 0x02
	#define allLEDsFlashTwice 0x03
	#define allLEDsFlashThrice 0x04
	#define allLEDsFlashFourTimes 0x05
	#define LED1on 0x06
	#define LED2on 0x07
	#define LED3on 0x08
	#define LED4on 0x09
	#define LEDRotating 0x0a
	#define LEDBlinking 0x0b
	#define LEDSlowBlinking 0x0c
	#define LEDAlternating 0x0d

#endif
