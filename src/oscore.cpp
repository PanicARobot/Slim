#include "OrientationSensorsWrapper.h"
#include "SDLogDriver.h"
#include "DualEncoderDriver.h"
#include "MotorDriver.h"

#include <Wire.h>
#include <SD.h>

#define LED_PIN 13

#define LEFT_SENSOR_PIN  A4
#define RIGHT_SENSOR_PIN A5

const uint32_t MICROS_PER_SEC = 1000000;

const uint32_t SAMPLE_FREQUENCY = 250;
const uint32_t PRINT_FREQUENCY = 5;

enum {
	WAITING_FOR_COMMAND,
	PREPARE_TO_FIGHT,
	IN_COMBAT,
	RECALIBRATE,
	BRAINDEAD,
	TEST
} RobotState = WAITING_FOR_COMMAND;

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
	position.calibrate();
}

void processCommand()
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

	if(right)
	{
		if(left > last_left)
		{
			++command_counter;
			Serial.print(command_counter);
		}
	}
	else if(right < last_right)
	{
		switch(command_counter)
		{
			case 0:
				RobotState = PREPARE_TO_FIGHT;
				prepare_micros = micros();
				break;
			case 1:
				RobotState = TEST;
				setMotors(80, 80);
				break;
			case 2:
				RobotState = RECALIBRATE;
				break;
		}

		command_counter = 0;
	}

	last_left = left;
	last_right = right;
}

void checkFailSafe()
{
	int left, right;
	readProximitySensors(left, right);
	if(left && right)
	{
		RobotState = BRAINDEAD;
		setMotors(0, 0);
	}
}

void loop()
{
	static uint32_t lastSampleMicros = 0;
	static uint32_t lastPrintMicros = 0;
	static uint32_t lastBlinkMicros = 0;

	uint32_t currentMicros = micros();

	if(RobotState == BRAINDEAD)
	{
		setMotors(0, 0);

		// Braindead blink
		if(currentMicros - lastBlinkMicros >= MICROS_PER_SEC * 2)
		{
			lastBlinkMicros = currentMicros;
			digitalWrite(LED_PIN, HIGH);
		}
		else if(currentMicros - lastBlinkMicros >= MICROS_PER_SEC)
		{
			digitalWrite(LED_PIN, LOW);
		}

		return;
	}

	if(RobotState == PREPARE_TO_FIGHT)
	{
		if(currentMicros - prepare_micros >= MICROS_PER_SEC * 5)
		{
			RobotState = IN_COMBAT;
		}
	}

	// Read sensors
	if(currentMicros - lastSampleMicros >= MICROS_PER_SEC / SAMPLE_FREQUENCY)
	{
		position.read_sensors();

		if(RobotState == WAITING_FOR_COMMAND)
			getCommand();
		else checkFailSafe();

		lastSampleMicros = currentMicros;
	}

	// Log data
	if(currentMicros - lastPrintMicros >= MICROS_PER_SEC / PRINT_FREQUENCY)
	{
		log_info();
		lastPrintMicros = currentMicros;
	}

	// Blink when working
	if(currentMicros - lastBlinkMicros >= MICROS_PER_SEC / 2)
	{
		lastBlinkMicros = currentMicros;
		digitalWrite(LED_PIN, HIGH);
	}
	else if(currentMicros - lastBlinkMicros >= MICROS_PER_SEC / 3)
	{
		digitalWrite(LED_PIN, LOW);
	}

	processCommand();
}
