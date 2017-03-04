/* @file	motor.cpp
 *
 * @brief	HSR-8498HB servo motor interface, implementation.
 *
 * @date 	10.12.2014
 * @autor 	Martin Gritzan
 */
#include "motor.h"
#include "unistd.h"
#include <math.h>

Motor::Motor(const int serialPortHandle,
			const char MotorId,  const float min, const float max){

	/* set motor ID, speed and port handle */
	motorId = MotorId;
	uartPortHandle = serialPortHandle;
	currentSpeed = 100;
	maxAngle = max;
	minAngle = min;

	return;
}

Motor::Motor(const char* uartPortDeviceFileName,
			const char MotorId, const float min, const float max){

	/* set motor id and speed */
	motorId = MotorId;
	currentSpeed = 100;
	maxAngle = max;
	minAngle = min;

	/* open com port */
	uartPortHandle = fnInitV24(uartPortDeviceFileName);
	/* set timeout to 0.5s */
	fnSetTimeout(uartPortHandle, 5);

	/* initialize communications */
	initalizeMotors(uartPortHandle);
	return;
}

Motor::~Motor(){
	return;
}

bool Motor::setSpeed(const char speed, bool debugOn){
	/* command for set speed */
	char command[8] = {0, 128, 233, motorId, speed, 0, 0, 0};

	/* calculate checksum and send */
	command[5] = calculateChecksum(command);

	if (debugOn) {
		std::cout << "Setting motor " << (int)motorId << " to speed " << (int)speed << std::endl;
		std::cout << "sending {";
		for (int i = 0; i < 8; i++)
			std::cout << (int) (command[i]) << "; ";
		std::cout << "}";
	}

	int bytesSent = fnWriteV24(uartPortHandle,command,8);
	if(debugOn) std::cout << " ("<< bytesSent << " bytes sent)" << std::endl;
	tcdrain(uartPortHandle);
	currentSpeed = speed;
	return true;
}

bool Motor::setAngle(const float angle, bool debugOn){
	/* convert angle to servo value */
	float angleDegFloat = fmod(angle * 57.2957795, 360);
	/* round to nearest full degree */
	short servoValue = ((short)(angleDegFloat + 0.5)+ 90) * 10 + 600;
	/* split to bytes */
	char servoValueLowByte = servoValue & 0xFF;
	char servoValueHighByte = (servoValue & 0xFF00) >> 8;
	/* command for set angle */
	char command[8] = {0, 128, motorId, servoValueHighByte,
						servoValueLowByte, 0, 0, 0};

	/* calculate checksum and send */
	command[5] = calculateChecksum(command);

	if(debugOn){
		std::cout << "Moving motor " << (int)motorId << " to "
				<< servoValue << " (" << angleDegFloat << " deg)"
				<< std::endl;
		std::cout << "sending {";
		for(int i = 0; i < 8; i++)
			std::cout << (int)(command[i]) << "; ";
		std::cout <<"}";
	}

	int bytesSent = fnWriteV24(uartPortHandle,command,8);
	if(debugOn) std::cout << " ("<< bytesSent << " bytes sent)" << std::endl;
	tcdrain(uartPortHandle);
	return true;
}

float Motor::getAngle(bool debugOn){
	char answer[8] = {0,0,0,0,0,0,0,0};
	int angle = 0;

	tcdrain(uartPortHandle);
	tcflush(uartPortHandle,TCIFLUSH);
	setSpeed(currentSpeed, debugOn);
	int bytesRead = fnReadV24(uartPortHandle, answer, 8);

	if(debugOn){
		std::cout << "{";
		for (int i = 0; i < 8; i++)
			std::cout << (int)(answer[i]) << "; ";
		std::cout << "} (" << bytesRead << " bytes) received." << std::endl;
	}

	angle = ( (int)(answer[6]) << 8 ) + (int)(answer[7]);
	return ((((float)angle - 600) / 10) - 90) / 57.2957795;
}

char Motor::calculateChecksum(char* command){
	unsigned char checksum = 0;
	for(int i = 0; i<8; i++){
		checksum += command[i];
	}
	checksum = checksum % 256;
	checksum = 256 - checksum;
	checksum = checksum % 256;
	return (char)checksum;
}

float Motor::checkAngle(float angle){
	if (angle > maxAngle) return maxAngle;
	if (angle < minAngle) return minAngle;
	return angle;
}

void Motor::initalizeMotors(int uartPortHandle){
	/* initialize communications: write to not existing motor */
	char command[8] = { 128, NON_EXISTING_MOTOR_ID, 135, 0, 0, 0, 0 , 0};
	command[5] = calculateChecksum(command);
	int bytesSent = fnWriteV24(uartPortHandle, command, 7);
	std::cout << " (" << bytesSent << " bytes sent) to init motors." << std::endl;
	tcdrain(uartPortHandle);
	sleep(1);
}





