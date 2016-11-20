#include "RobotStateControl.h"

#include "OrientationSensorsWrapper.hpp"
#include "SDLogDriver.hpp"
#include "DualEncoderDriver.hpp"
#include "MotorDriver.h"
#include "TireContactModule.hpp"
#include "FrontLiftedDetection.hpp"
#include "MainLogic.h"
#include "ProximitySensors.hpp"
#include "SerialCommander.h"
#include "planar/PlanarAccelerationModule.hpp"
#include "planar/PlanarSpeedModule.hpp"

#include <Wire.h>
#include <SD.h>

#define SERIAL_BAUD_RATE        115200
#define I2C_FREQUENCY           400000
#define SD_CHIP_SELECT          4

#define LOG_FREQUENCY           100

#define SAMPLE_FREQUENCY        250
#define MICROS_PER_SECOND       1000000

#define LED_PIN                 13

#define LEFT_A_PIN              12
#define LEFT_B_PIN              11
#define RIGHT_A_PIN             9
#define RIGHT_B_PIN             10

enum {
	WAITING_FOR_COMMAND,
	PREPARE_TO_FIGHT,
	FIGHT_MODE,
	BRAINDEAD,
	TEST_MODE
} robot_state = WAITING_FOR_COMMAND;

OrientationSensors position;

DualEncoder leftEncoder(LEFT_A_PIN, LEFT_B_PIN);
DualEncoder rightEncoder(RIGHT_A_PIN, RIGHT_B_PIN);

void setup()
{
	Serial.begin(SERIAL_BAUD_RATE);

	SD.begin(SD_CHIP_SELECT);

	initLogger();

	leftEncoder.init([](){leftEncoder.A_handler();}, [](){leftEncoder.B_handler();});
	rightEncoder.init([](){rightEncoder.A_handler();}, [](){rightEncoder.B_handler();});

	initMotors();

	initProximitySensors();

	Wire.begin();
	Wire.setClock(I2C_FREQUENCY);

	position.init();
}

uint32_t event_micros;

void calibrate() {
	digitalWrite(LED_PIN, HIGH);
	position.calibrate();
	initPlanarSpeed();
}

void setState(int new_state) {
	robot_state = (decltype(robot_state)) new_state;
}

void getProximityCommand()
{
	static uint8_t left = 0;
	static uint8_t right = 0;

	static uint8_t last_left = 0;
	static uint8_t last_right = 0;

	static int command_counter = 0;

	readProximitySensors(left, right);

	if(robot_state == WAITING_FOR_COMMAND)
	{
		if(right)
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
						event_micros = micros();
						break;

					case 1:
						calibrate();
						break;

					case 2:
						robot_state = TEST_MODE;
						Serial.println("Welcome to test mode");
						break;
				}
			}

			command_counter = 0;
		}
	}
	else if(left && right)
	{
		if(robot_state == FIGHT_MODE)
		{
			robot_state = BRAINDEAD;
		}
		else
		{
			robot_state = WAITING_FOR_COMMAND;
			command_counter = 42; // Just something invalid and BIG
		}

		robot_state = WAITING_FOR_COMMAND; // Remove
		setMotors(0, 0);
	}

	last_left = left;
	last_right = right;
}

void loop()
{
	static uint32_t last_sample_micros = 0;
	static uint32_t last_log_micros = 0;
	static uint32_t last_blink_micros = 0;

	uint32_t current_micros = micros();

	{ // blink control
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

	switch(robot_state)
	{
		case BRAINDEAD:
			setMotors(0, 0);
			return;

		case PREPARE_TO_FIGHT:
			if(current_micros - event_micros >= MICROS_PER_SECOND * 5)
			{
				robot_state = FIGHT_MODE;
			}
			break;
		case FIGHT_MODE:
			{
				if(current_micros - last_sample_micros >= MICROS_PER_SECOND / SAMPLE_FREQUENCY)
				{
					/*if(getLeftTireContactState() == 0 && getRightTireContactState() == 0)
					  {
					  robot_state = BRAINDEAD;
					  break;
					  }*/

					MainLogic(leftEncoder.getSpeed(), rightEncoder.getSpeed());
				}
			}
			break;

		case TEST_MODE:
			if(current_micros - last_sample_micros >= MICROS_PER_SECOND / SAMPLE_FREQUENCY)
			{
				/* PERFORM TESTS */
			}
			break;

		default:
			break;
	}

	// Read sensors
	if(current_micros - last_sample_micros >= MICROS_PER_SECOND / SAMPLE_FREQUENCY)
	{
		position.update();
		leftEncoder.update();
		rightEncoder.update();
		updatePlanarSpeed(position);

		getProximityCommand();

		last_sample_micros = current_micros;
	}

	// Log data
	if(current_micros - last_log_micros >= MICROS_PER_SECOND / LOG_FREQUENCY)
	{
		logDataPack(position, leftEncoder, rightEncoder, acceleration_vector, speed_vector);
		last_log_micros = current_micros;
	}

	getSerialCommand();
}
