#include <Wire.h>
#include "OrientationSensorsWrapper.h"

#include <SD.h>
#include "SDLogDriver.h"

#define LED_PIN 13

const uint32_t MICROS_PER_SEC = 1000000;

const uint32_t SAMPLE_FREQUENCY = 250;
const uint32_t PRINT_FREQUENCY = 5;

OrientationSensors position;

void log_info()
{
	Serial.print("ACC: ");
	Serial.print(position.getAccX());
	Serial.print(" ");
	Serial.print(position.getAccY());
	Serial.print(" ");
	Serial.print(position.getAccZ());
	Serial.print("   Gyro: ");
	Serial.print(position.getGyroX());
	Serial.print(" ");
	Serial.print(position.getGyroY());
	Serial.print(" ");
	Serial.print(position.getGyroZ());
	Serial.print("   ");

	Serial.print("AHRS: ");
	Serial.print(position.getRoll() * 180 / M_PI);
	Serial.print(" ");
	Serial.print(position.getPitch() * 180 / M_PI);
	Serial.print(" ");
	Serial.print(position.getYaw() * 180 / M_PI);
	Serial.println("");
}

void setup()
{
	Serial.begin(115200);

	SD.begin(4);

	Wire.begin();
	Wire.setClock(400000);

	position.init();
	position.calibrate();
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
}
