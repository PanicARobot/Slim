#include "SerialCommander.h"

#include "RobotStateControl.h"
#include "../drivers/SDLogDriver.hpp"
#include "../drivers/MotorDriver.h"

#include <Wire.h>

void getSerialCommand()
{
	if(Serial.available() == 0) return;

	static int left = 0;
	static int right = 0;

	char cmd = Serial.read();

	if(cmd == 'l' || cmd == 'r' || cmd == 'b')
	{
		int speed = 0;
		if(Serial.available() > 0)
		{
			int dir = 1;
			if(Serial.peek() == '-')
			{
				dir = -1;
				Serial.read();
			}
			while(Serial.available() > 0)
			{
				char digit = Serial.read();
				speed = speed * 10 + (digit - '0');
			}
			speed *= dir;
		}

		if(cmd == 'l' || cmd == 'b') left = speed;
		if(cmd == 'r' || cmd == 'b') right = speed;

		setMotors(left, right);
	}
	else if(cmd == 'd') {
		dumpLog();
	}
	else if(cmd == 'e') {
		dropLog();
	}
	else if(cmd == 'h') {
		Serial.println("Hello, I am here!");
	}
	else if(cmd == 'm') {
		int new_state = 0;
		if(Serial.available() > 0)
			new_state = Serial.read() - '0';

		setState(new_state);
	}
	else if(cmd == 'q') {
		void (*reset)(void) = 0;
		reset();
	}
}
