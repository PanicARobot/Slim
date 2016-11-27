#include "RobotStateControl.h"

#include "../drivers/MotorDriver.h"
#include "../drivers/ProximitySensors.hpp"
#include "../drivers/StartModule.h"

#include <Arduino.h>
#include <Wire.h>

#define LED_PIN 13
#define PREPARE_TO_FIGHT_TIMEOUT 5

enum {
	WAITING_FOR_COMMAND, // 0
	CALIBRATE,           // 1
	PREPARE_TO_FIGHT,    // 2
	FIGHT_MODE,          // 3
	TEST_MODE,           // 4
	BRAINDEAD = 9        // 9
} robot_state = WAITING_FOR_COMMAND;

void setState(int new_state) {
	robot_state = (decltype(robot_state)) new_state;
}

void updateRobotState(uint32_t current_micros) {
	static uint8_t left = 0;
	static uint8_t right = 0;

	static uint8_t last_left = 0;
	static uint8_t last_right = 0;

	static int command_counter = 0;

	static uint32_t start_time_micros;

	readProximitySensors(left, right);

	if(robot_state == WAITING_FOR_COMMAND) {
		if(getStart())
		{
			robot_state = FIGHT_MODE;
		}
		else if(right)
		{
			if(left < last_left)
			{
				++command_counter;
				Serial.print("Counter: ");
				Serial.println(command_counter);
			}
		}
		else if(right < last_right)
		{
			if(!left)
			{
				switch(command_counter)
				{
					case 0:
						robot_state = PREPARE_TO_FIGHT;
						start_time_micros = current_micros;
						break;

					case 1:
						robot_state = CALIBRATE;
						break;

					case 2:
						robot_state = TEST_MODE;
						break;
				}
			}

			command_counter = 0;
		}
	}
	else if(left && right)
	{
		if(robot_state == FIGHT_MODE) {
			robot_state = BRAINDEAD;
		}
		else if(robot_state != BRAINDEAD) {
			robot_state = WAITING_FOR_COMMAND;
			command_counter = 42; // Just something invalid and BIG
		}

		setMotors(0, 0);
	}
	else if(robot_state == PREPARE_TO_FIGHT) {
		if(current_micros - start_time_micros >= MICROS_PER_SECOND * PREPARE_TO_FIGHT_TIMEOUT)
		{
			robot_state = FIGHT_MODE;
		}
	}

	last_left = left;
	last_right = right;
}

void indicateRobotState(uint32_t current_micros) {
	static uint32_t last_blink_micros = 0;

	uint32_t full_cycle = MICROS_PER_SECOND;
	uint32_t high_cycle = MICROS_PER_SECOND;

	switch(robot_state)
	{
		case BRAINDEAD:
			full_cycle *= 2;
			high_cycle *= 1;
			break;

		case PREPARE_TO_FIGHT:
			full_cycle /= 10;
			high_cycle /= 20;
			break;

		case FIGHT_MODE:
		case TEST_MODE:
			full_cycle /= 2;
			high_cycle /= 3;

		default:
			full_cycle /= 2;
			high_cycle /= 5;
			break;
	}

	if(current_micros - last_blink_micros >= full_cycle)
	{
		digitalWrite(LED_PIN, HIGH);
		last_blink_micros = current_micros;
	}
	else if(current_micros - last_blink_micros >= high_cycle)
	{
		digitalWrite(LED_PIN, LOW);
	}
}

void handleRobotAction(void (*calibrate)(), void (*fight)(), void (*test)()) {
	switch(robot_state)
	{
		case BRAINDEAD:
		case PREPARE_TO_FIGHT:
			setMotors(0, 0);
			break;

		case CALIBRATE:
			setMotors(0, 0);
			digitalWrite(LED_PIN, HIGH);

			Serial.println("Calibrating...");
			calibrate();
			Serial.println("Calibrated!");

			robot_state = WAITING_FOR_COMMAND;
			break;

		case FIGHT_MODE:
			fight();
			break;

		case TEST_MODE:
			test();
			break;

		default:
			break;
	}
}
