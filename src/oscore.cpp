#include "OrientationSensorsWrapper.h"
#include "SDLogDriver.h"
#include "DualEncoderDriver.h"
#include "MotorDriver.h"
#include "FrontLiftedDetection.h"

#include "constants.h"

#include <Wire.h>
#include <SD.h>

enum {
	WAITING_FOR_COMMAND,
	PREPARE_TO_FIGHT,
	IN_COMBAT,
	RECALIBRATE,
	BRAINDEAD,
	TEST
} robot_state = WAITING_FOR_COMMAND;

uint32_t prepare_micros;

void log_info()
{
	log_data_pack.timestamp = millis();

	log_data_pack.acc_x = position.getAccX();
	log_data_pack.acc_y = position.getAccY();
	log_data_pack.acc_z = position.getAccZ();

	log_data_pack.gyro_x = position.getGyroX();
	log_data_pack.gyro_y = position.getGyroY();
	log_data_pack.gyro_z = position.getGyroZ();

	log_data_pack.ahrs_x = position.getRoll();
	log_data_pack.ahrs_y = position.getPitch();
	log_data_pack.ahrs_z = position.getYaw();

	log_data_pack.leftSpeed = leftEncoder.getSpeed();
	log_data_pack.rightSpeed = rightEncoder.getSpeed();

	logDataPack();
}

void setup()
{
	Serial.begin(115200);

	SD.begin(4);

	initLogger();

	initDualEncoders();
	initMotors();

	pinMode(LEFT_SENSOR_PIN, INPUT);
	pinMode(RIGHT_SENSOR_PIN, INPUT);

	Wire.begin();
	Wire.setClock(400000);

	position.init();

	tiresContactInit(TIRE_LOST_OF_CONTACT_DEGREES);

	initFrontLifted(FRONT_LIFTED_THRESHOLD);
}

void serialCommand()
{
	static int left = 0;
	static int right = 0;

	if(Serial.available() == 0) return;

	char cmd = Serial.read();
	if(cmd == 'l')
	{
		left = 0;
		if(Serial.available() > 0)
		{
			char dir = Serial.read();
			while(Serial.available() > 0)
			{
				char digit = Serial.read();
				left = left * 10 + (digit - '0');
			}

			if(dir == '-') left = -left;
		}

		setMotors(left, right);
	}
	else if(cmd == 'r')
	{
		right = 0;
		if(Serial.available() > 0)
		{
			char dir = Serial.read();
			while(Serial.available() > 0)
			{
				char digit = Serial.read();
				right = right * 10 + (digit - '0');
			}

			if(dir == '-') right = -right;
		}

		setMotors(left, right);
	}
	else if(cmd == 'd')
	{
		dumpLog();
	}
}

void readProximitySensors(int& left, int& right)
{
	left = digitalRead(LEFT_SENSOR_PIN) ^ 1;
	right = digitalRead(RIGHT_SENSOR_PIN) ^ 1;
}

void getCommand()
{
	static int left = 0;
	static int right = 0;

	static int last_left = 0;
	static int last_right = 0;

	static int command_counter = 0;

	readProximitySensors(left, right);

	if(robot_state == WAITING_FOR_COMMAND)
	{
		if(right)
		{
			if(left > last_left)
			{
				++command_counter;
				Serial.print("Command counter: ");
				Serial.println(command_counter);
			}
		}
		else if(right < last_right)
		{
			switch(command_counter)
			{
				case 0:
					robot_state = PREPARE_TO_FIGHT;
					prepare_micros = micros();
					break;
				case 1:
					robot_state = RECALIBRATE;
					position.calibrate();
					robot_state = WAITING_FOR_COMMAND;
					break;
				case 2:
					robot_state = TEST;
					setMotors(80, 80);
					break;
			}

			command_counter = 0;
		}
	}
	else if(left && right)
	{
		if(robot_state == PREPARE_TO_FIGHT)
			robot_state = WAITING_FOR_COMMAND;
		else
		{
			robot_state = BRAINDEAD;
			setMotors(0, 0);
		}
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
		uint32_t full_cycle;
		uint32_t high_cycle;

		if(robot_state == BRAINDEAD)
		{
			full_cycle = MICROS_PER_SECOND * 2;
			high_cycle = MICROS_PER_SECOND;
		}
		else if(robot_state == PREPARE_TO_FIGHT)
		{
			full_cycle = MICROS_PER_SECOND / 10;
			high_cycle = MICROS_PER_SECOND / 20;
		}
		else
		{
			full_cycle = MICROS_PER_SECOND / 2;
			high_cycle = MICROS_PER_SECOND / 3;
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

	if(robot_state == BRAINDEAD)
	{
		setMotors(0, 0);
		return;
	}

	if(robot_state == PREPARE_TO_FIGHT)
	{
		if(current_micros - prepare_micros >= MICROS_PER_SECOND * 5)
		{
			robot_state = IN_COMBAT;
		}
	}

	// Read sensors
	if(current_micros - last_sample_micros >= MICROS_PER_SECOND / SAMPLE_FREQUENCY)
	{
		position.read_sensors();

		getCommand();

		last_sample_micros = current_micros;
	}

	// Log data
	if(current_micros - last_log_micros >= MICROS_PER_SECOND / LOG_FREQUENCY)
	{
		log_info();
		last_log_micros = current_micros;
	}

	serialCommand();
}
