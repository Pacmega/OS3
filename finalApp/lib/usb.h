#ifndef _USB_H_
#define _USB_H_

// 128  64  32  16  8  4  2  1
//  7    6   5   4  3  2  1  0

	#define _BV(x) (1 << x)

	#define messageType        0
	#define packetSize         1

	// Group 1
	#define inputGroup1        2

	#define rightStick         7
	#define leftStick          6
	#define backButton         5
	#define startButton        4

	#define dpadRight          3
	#define dpadLeft           2
	#define dpadDown           1
	#define dpadUp 		       0

	#define rightStickPressed  128
	#define leftStickPressed   64
	#define backButtonPressed  32
	#define startButtonPressed 16

	#define dpadRightPressed   8
	#define dpadLeftPressed    4
	#define dpadDownPressed    2
	#define dpadUpPressed      1

	// Group 2
	#define inputGroup2        3

	#define buttonY            7
	#define buttonX            6
	#define buttonB            5
	#define buttonA            4

	#define buttonXBOX         2
	#define rightShoulder      1
	#define leftShoulder       0

	// No bit shifting values for the triggers, they have an
	// entire byte dedicated to them.

	#define buttonYPressed        128
	#define buttonXPressed        64
	#define buttonBPressed        32
	#define buttonAPressed        16

	#define buttonXBOXPressed     4
	#define rightShoulderPressed  2
	#define leftShoulderPressed   1

	// Triggers
	#define leftTriggerPressed    4
	#define rightTriggerPressed   5

	// LED modes
	#define allLEDsOff            0x00
	#define allLEDsBlinking       0x01
	#define allLEDsFlashOnce      0x02
	#define allLEDsFlashTwice     0x03
	#define allLEDsFlashThrice    0x04
	#define allLEDsFlashFourTimes 0x05
	#define LED1on                0x06
	#define LED2on                0x07
	#define LED3on                0x08
	#define LED4on                0x09
	#define LEDRotating           0x0a
	#define LEDBlinking           0x0b
	#define LEDSlowBlinking       0x0c
	#define LEDAlternating        0x0d

#endif
