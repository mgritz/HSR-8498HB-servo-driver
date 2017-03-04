/* @file	motor.h
 *
 * @brief	HSR-8498HB servo motor interface class, declarations.
 *
 * @date 	10.12.2014
 * @autor 	Martin Gritzan
 */

#ifndef MOTOR_H_
#define MOTOR_H_

#define MAGICWORD 128
#define COMFAULT 1
#define SUCCESS 0
#define FAILED -1
#define NON_EXISTING_MOTOR_ID 20

#include "serial.h"

#include <iostream>
using namespace std;



/** \brief Interface class for HSR-8498HB servo motor.
 *
 * This class communicates with a HSR-8498HB servo motor connected to a
 * serial (UART) port.
 */
class Motor{

public:
	/** Constructor to be used if serial port already open.
	 *
	 * @param serialPortHandle Handle of the UART port, obtained from driver.
	 * @param MotorId ID this motor is listening to.
	 * @param min Minimum allowed motor angle in [rad].
	 * @param max Maximum allowed motor angle in [rad]. */
	Motor(	const int serialPortHandle,
			const char MotorId, const float min, const float max);

	/** Constructor which also initializes the serial port.
	 *
	 * @param uartPortDeviceFileName UNIX device file name, e.g. "/dev/ttySAC0".
	 * @param MotorId ID this motor is listening to.
	 * @param min Minimum allowed motor angle in [rad].
	 * @param max Maximum allowed motor angle in [rad]. */
	Motor(	const char* uartPortDeviceFileName,
			const char MotorId, const float min, const float max);


	/** Destructor, leaves the serial port open! */
	~Motor();

	/** Sets the motor's speed argument.
	 * @param speed Percentage value for motor speed.
	 * @param debugOn Debug messages on.
	 * @returns allways true. */
	bool setSpeed(const char speed, bool debugOn = false);

	/** Moves the motor to an new angle.
	 * @param angle Absolute new motor angle in [rad].
	 * @param debugOn Debug messages on.
	 * @returns allways true. */
	bool setAngle(const float angle, bool debugOn = false);

	/** Gets the motor's current angle.
	 * @param debugOn Debug messages on.
	 * @returns the current angle in radians. */
	float getAngle(bool debugOn = false);

	/** Coerces angle into allowed range. Nothing is communicated.
	 * @returns the allowed angle. */
	float checkAngle(const float angle);

	/** @returns the motor ID. */
	char getMotorId(){return motorId;};

	/** Initializes motors by sending nonsense command over serial i'face
	 * @param uartPortHandle Handle for the serial port, obtained from driver.*/
	static void initalizeMotors(int uartPortHandle);

private:
	Motor(){};	// forbidden

	/** Calculates the checksum for a command string.
	 *
	 * Usage:
	 *
	 *      char array[8] = {...};
	 *      array[5] = calculateChecksum(array);
	 *
	 * @param command Array of 8 character command, checksum byte is zero.
	 * @returns The checksum byte. */
	static char calculateChecksum(char* command);

	char motorId;		//!< local motor ID
	char currentSpeed;	//!< currently set speed
	int uartPortHandle;	//!< serial port handle
	float maxAngle;	//!< minimum angle allowed for this motor
	float minAngle; //!< maximum angle allowed for this motor
};
#endif /* MOTOR_H_ */
