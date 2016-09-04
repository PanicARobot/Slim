#include "OrientationSensorsWrapper.h"
#include "SDLogDriver.h"
#include "DualEncoderDriver.h"
#include "MotorDriver.h"

#include <Wire.h>
#include <SD.h>

#define LED_PIN 13


const uint32_t MICROS_PER_SEC = 1000000;

const uint32_t SAMPLE_FREQUENCY = 250;
const uint32_t PRINT_FREQUENCY = 5;

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

void loop()
{
	static uint32_t lastSampleMicros = 0;
	static uint32_t lastPrintMicros = 0;
	static uint32_t lastBlinkMicros = 0;

	uint32_t currentMicros = micros();

	// Read orientation sensors
	if(currentMicros - lastSampleMicros >= MICROS_PER_SEC / SAMPLE_FREQUENCY)
	{
		position.read_sensors();
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
