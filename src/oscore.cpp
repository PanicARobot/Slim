#include <Wire.h>
#include "OrientationSensorsWrapper.h"

const uint32_t MICROS_PER_SEC = 1000000;

const uint32_t SAMPLE_FREQUENCY = 250;
const uint32_t PRINT_FREQUENCY = 5;

OrientationSensors position;

int cyclesPerSecond = 0;

void print_info()
{
	//float gyroX = imu.g.x / dpsScale;
	//float gyroY = imu.g.y / dpsScale;
	//float gyroZ = imu.g.z / dpsScale;

	//float accX = imu.a.x / gScale;
	//float accY = imu.a.y / gScale;
	//float accZ = imu.a.z / gScale;

	Serial.print(cyclesPerSecond * PRINT_FREQUENCY);
	Serial.print("\t");
	cyclesPerSecond = 0;

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
	Serial.print(position.getRoll());
	Serial.print(" ");
	Serial.print(position.getPitch());
	Serial.print(" ");
	Serial.print(position.getYaw());
	Serial.println("");
}

void setup()
{
	Serial.begin(115200);

	Wire.begin();
	Wire.setClock(400000);

	position.init();
	position.calibrate();
}

uint32_t lastSampleMicros = 0;
uint32_t lastPrintMicros = 0;

void loop()
{
	uint32_t currentMicros = micros();

	if(currentMicros - lastSampleMicros >= MICROS_PER_SEC / SAMPLE_FREQUENCY)
	{
		position.read_sensors();
		++cyclesPerSecond;
		lastSampleMicros = currentMicros;
	}

	if(currentMicros - lastPrintMicros >= MICROS_PER_SEC / PRINT_FREQUENCY)
	{
		print_info();
		lastPrintMicros = currentMicros;
	}
}
