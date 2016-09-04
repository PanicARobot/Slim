#include "MotorDriver.h"

#include <Arduino.h>

#define MOTOR_LEFT_DIR_PIN  15
#define MOTOR_LEFT_PWM_PIN  16
#define MOTOR_RIGHT_DIR_PIN 6
#define MOTOR_RIGHT_PWM_PIN 5

void initMotors(void)
{
	// init all outputs as outputs....
	pinMode(MOTOR_LEFT_DIR_PIN, OUTPUT);
	pinMode(MOTOR_LEFT_PWM_PIN, OUTPUT);
	pinMode(MOTOR_RIGHT_DIR_PIN, OUTPUT);
	pinMode(MOTOR_RIGHT_PWM_PIN, OUTPUT);

	// set PWM to default value
	setMotors(0, 0);
}

void setMotor(int32_t speed, int dirPin, int speedPin)
{
	static constexpr int32_t MAX_SPEED = 255;

	if(speed > MAX_SPEED) speed = MAX_SPEED;
	else if(speed < -MAX_SPEED) speed = -MAX_SPEED;

	digitalWrite(dirPin, speed > 0);
	if(speed < 0) speed = -speed;
	analogWrite(speedPin, speed);
}

void setMotors(int32_t left, int32_t right)
{
	setMotor(left, MOTOR_LEFT_DIR_PIN, MOTOR_LEFT_PWM_PIN);
	setMotor(right, MOTOR_RIGHT_DIR_PIN, MOTOR_RIGHT_PWM_PIN);
}
