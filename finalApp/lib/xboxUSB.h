#ifndef _USB_H_
#define _USB_H_

	// Should have the same functionality as the Arduino _BV macro.
	#define _BV(x) (1 << x)

	#define messageType        0		// Position in the input report
	#define packetSize         1		// Position in the input report

	// Group 1
	#define inputGroup1        2		// Position in the input report

	// Bit location within the group
	#define rightStick         7
	#define leftStick          6
	#define backButton         5
	#define startButton        4

	#define dpadRight          3
	#define dpadLeft           2
	#define dpadDown           1
	#define dpadUp 		       0

	// Values to check against after selecting the one bit from the input report
	#define rightStickPressed  128
	#define leftStickPressed   64
	#define backButtonPressed  32
	#define startButtonPressed 16

	#define dpadRightPressed   8
	#define dpadLeftPressed    4
	#define dpadDownPressed    2
	#define dpadUpPressed      1

	// Group 2
	#define inputGroup2        3		// Position in the input report

	// Bit location within the group
	#define buttonY            7
	#define buttonX            6
	#define buttonB            5
	#define buttonA            4

	#define buttonXBOX         2
	#define rightShoulder      1
	#define leftShoulder       0

	// Values to check against after selecting the one bit from the input report
	#define buttonYPressed        128
	#define buttonXPressed        64
	#define buttonBPressed        32
	#define buttonAPressed        16

	#define buttonXBOXPressed     4
	#define rightShoulderPressed  2
	#define leftShoulderPressed   1

	// Triggers
	#define leftTrigger           4		// Position in the input report
	#define rightTrigger          5		// Position in the input report

	#define noPower				  0
	#define mediumPower			  122
	#define fullPower			  255

	// No other values need to be defined for the triggers,
	// they have an entire byte dedicated to them.

	#define leftStickPt1		  6
	#define leftStickPt2		  7
	#define leftStickPt3		  8
	#define leftStickPt4		  9
	#define rightStickPt1		  10
	#define rightStickPt2		  11
	#define rightStickPt3		  12
	#define rightStickPt4		  13

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
