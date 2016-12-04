#include "RobotStateControl.h"

#include "UICommander.h"

#include "../drivers/MotorDriver.h"
#include "../drivers/StartModule.h"
#include "../logic/fight.h"
#include "../logic/test.h"

#include <Arduino.h>

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

void setState(uint32_t state) {
	robot_state = state;
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

void handleRobotAction(uint32_t current_micros, void (*calibrate)(void)) {
	static uint32_t start_time_micros;

	switch(robot_state)
	{
		case WAITING_FOR_COMMAND:
			setMotors(0, 0);

			if(remoteStarted())
			{
				initFight();
				robot_state = FIGHT_MODE;
				break;
			}

			int8_t command = getUICommand();
			if(command == 0)
			{
				robot_state = PREPARE_TO_FIGHT;
				start_time_micros = current_micros;
			}
			else if(command == 1)
			{
				robot_state = CALIBRATE;
			}
			else if(command == 2)
			{
				initTest();
				robot_state = TEST_MODE;
			}
			break;

		case PREPARE_TO_FIGHT:
			if(getUICommand() == UI_FAILSAFE)
			{
				robot_state = WAITING_FOR_COMMAND;
			}
			else if(current_micros - start_time_micros >= MICROS_PER_SECOND * PREPARE_TO_FIGHT_TIMEOUT)
			{
				initFight();
				robot_state = FIGHT_MODE;
				break;
			}

		case BRAINDEAD:
			setMotors(0, 0);
			break;

		case CALIBRATE:
			setMotors(0, 0);
			digitalWrite(LED_PIN, HIGH);

			calibrate();

			robot_state = WAITING_FOR_COMMAND;
			break;

		case FIGHT_MODE:
			if(remoteStarted() && getUICommand() != UI_FAILSAFE)
			{
				handleFight();
			}
			else
			{
				resetMovement();
				robot_state = BRAINDEAD;
			}
			break;

		case TEST_MODE:
			if(getUICommand() != UI_FAILSAFE)
			{
				handleTest();
			}
			else
			{
				resetMovement();
				robot_state = WAITING_FOR_COMMAND;
			}
			break;

		default:
			break;
	}
}
