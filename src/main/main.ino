#include <Wire.h>
#include "LSM6.h"

#include "MahonyAHRS.h"

LSM6 imu;
Mahony ahrs;

const uint32_t MICROS_PER_SEC = 1000000;

const uint32_t SAMPLE_FREQUENCY = 1660;
const uint32_t PRINT_FREQUENCY = 5;

const float gScale = 0x7FFF / (float)8;
const float dpsScale = 0x7FFF / (float)1000;

void setup()
{
	Serial.begin(115200);
	Wire.begin();

	if(!imu.init())
	{
		Serial.println("Failed to detect and initialize IMU!");

		delay(500);
		void (*f)() = nullptr;
		f();
		while(1);
	}

	imu.enableDefault();

	imu.writeReg(LSM6::CTRL1_XL, // Accelerometer
		0x8 << 4 | // 1.66 kHz sample frequency
		0x3 << 2 | // 8g
		0x1);      // Anti-aliasing: 200 Hz
	imu.writeReg(LSM6::CTRL2_G, // Gyroscope
		0x8 << 4 | // 1.66 kHz sample frequency
		0x2 << 2 | // 1000 degrees per second
		0x0);

	ahrs.begin((float) SAMPLE_FREQUENCY);
}

void make_sample()
{
	imu.read();

	float gyroX = imu.g.x / dpsScale;
	float gyroY = imu.g.y / dpsScale;
	float gyroZ = imu.g.z / dpsScale;

	float accX = imu.a.x / gScale;
	float accY = imu.a.y / gScale;
	float accZ = imu.a.z / gScale;

	ahrs.updateIMU(gyroX, gyroY, gyroZ, accX, accY, accZ);
}

char report[80];

void print_info()
{
	float gyroX = imu.g.x / dpsScale;
	float gyroY = imu.g.y / dpsScale;
	float gyroZ = imu.g.z / dpsScale;

	float accX = imu.a.x / gScale;
	float accY = imu.a.y / gScale;
	float accZ = imu.a.z / gScale;

	Serial.print("ACC: ");
	Serial.print(accX);
	Serial.print(" ");
	Serial.print(accY);
	Serial.print(" ");
	Serial.print(accZ);
	Serial.print("   Gyro: ");
	Serial.print(gyroX);
	Serial.print(" ");
	Serial.print(gyroY);
	Serial.print(" ");
	Serial.print(gyroZ);
	Serial.print("   ");

	float roll = ahrs.getRoll();
	float pitch = ahrs.getPitch();
	float yaw = ahrs.getYaw();
	
	Serial.print("AHRS: ");
	Serial.print(roll);
	Serial.print(" ");
	Serial.print(pitch);
	Serial.print(" ");
	Serial.print(yaw);
	Serial.println("");
}

uint32_t lastSampleMicros = 0;
uint32_t lastPrintMicros = 0;

void loop()
{
	uint32_t currentMicros = micros();

	if(currentMicros - lastSampleMicros >= MICROS_PER_SEC / SAMPLE_FREQUENCY)
	{
		make_sample();
		lastSampleMicros = currentMicros;
	}

	if(currentMicros - lastPrintMicros >= MICROS_PER_SEC / PRINT_FREQUENCY)
	{
		print_info();
		lastPrintMicros = currentMicros;
	}
}
