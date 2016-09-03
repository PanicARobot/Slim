#include <Wire.h>
#include "LSM6.h"

#include "MahonyAHRS.h"

LSM6 imu;
Mahony ahrs;

const uint32_t MICROS_PER_SEC = 1000000;

const uint32_t SAMPLE_FREQUENCY = 250;
const uint32_t PRINT_FREQUENCY = 5;

const float gScale = 0x7FFF / (float)8;
const float dpsScale = 0x7FFF / (float)1000;

int gyro_x_offset;
int gyro_y_offset;
int gyro_z_offset;

void calibrate_gyroscope()
{
	int samples_count = 0;

	int64_t gyro_x_sum = 0;
	int64_t gyro_y_sum = 0;
	int64_t gyro_z_sum = 0;

	// Collect sensor data for a second
	uint32_t startMicros = micros();
	while(micros() - startMicros < MICROS_PER_SEC)
	{
		imu.read();

		gyro_x_sum += imu.g.x;
		gyro_y_sum += imu.g.y;
		gyro_z_sum += imu.g.z;

		++samples_count;
	}

	gyro_x_offset = gyro_x_sum / samples_count;
	gyro_y_offset = gyro_y_sum / samples_count;
	gyro_z_offset = gyro_z_sum / samples_count;
}

void read_sensors()
{
	imu.read();

	// Apply gyroscope offsets
	imu.g.x -= gyro_x_offset;
	imu.g.y -= gyro_y_offset;
	imu.g.z -= gyro_z_offset;
}

void update_ahrs()
{
	float gyroX = imu.g.x / dpsScale;
	float gyroY = imu.g.y / dpsScale;
	float gyroZ = imu.g.z / dpsScale;

	float accX = imu.a.x / gScale;
	float accY = imu.a.y / gScale;
	float accZ = imu.a.z / gScale;

	ahrs.updateIMU(gyroX, gyroY, gyroZ, accX, accY, accZ);
}

float ahrs_roll_offset;
float ahrs_pitch_offset;
float ahrs_yaw_offset;

void calibrate_ahrs()
{
	{
		uint32_t startMicros = micros();
		uint32_t lastSampleMicros = startMicros - MICROS_PER_SEC / SAMPLE_FREQUENCY;

		while(true)
		{
			uint32_t currentMicros = micros();
			if(currentMicros - startMicros > MICROS_PER_SEC)
				break;

			if(currentMicros - lastSampleMicros < MICROS_PER_SEC / SAMPLE_FREQUENCY)
				continue;

			lastSampleMicros = currentMicros;

			read_sensors();
			update_ahrs();
		}
	}

	int samples_count = 0;

	float roll_sum = 0;
	float pitch_sum = 0;
	float yaw_sum = 0;

	// Collect data for a second
	uint32_t startMicros = micros();
	uint32_t lastSampleMicros = startMicros - MICROS_PER_SEC / SAMPLE_FREQUENCY;

	while(true)
	{
		uint32_t currentMicros = micros();
		if(currentMicros - startMicros > MICROS_PER_SEC)
			break;

		if(currentMicros - lastSampleMicros < MICROS_PER_SEC / SAMPLE_FREQUENCY)
			continue;

		lastSampleMicros = currentMicros;

		read_sensors();
		update_ahrs();

		roll_sum += ahrs.getRollRadians();
		pitch_sum += ahrs.getPitchRadians();
		yaw_sum += ahrs.getYawRadians();

		++samples_count;
	}

	Serial.println(roll_sum);

	ahrs_roll_offset = roll_sum / samples_count;
	ahrs_pitch_offset = pitch_sum / samples_count;
	ahrs_yaw_offset = yaw_sum / samples_count;
}

int cyclesPerSecond = 0;

void print_info()
{
	float gyroX = imu.g.x / dpsScale;
	float gyroY = imu.g.y / dpsScale;
	float gyroZ = imu.g.z / dpsScale;

	float accX = imu.a.x / gScale;
	float accY = imu.a.y / gScale;
	float accZ = imu.a.z / gScale;

	Serial.print(cyclesPerSecond * PRINT_FREQUENCY);
	Serial.print("\t");
	cyclesPerSecond = 0;

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

	float roll = ahrs.getRoll() - ahrs_roll_offset * 180 / M_PI;
	float pitch = ahrs.getPitch() - ahrs_pitch_offset * 180 / M_PI;
	float yaw = ahrs.getYaw() - ahrs_yaw_offset * 180 / M_PI;
	
	Serial.print("AHRS: ");
	Serial.print(roll);
	Serial.print(" ");
	Serial.print(pitch);
	Serial.print(" ");
	Serial.print(yaw);
	Serial.println("");
}

void setup()
{
	Serial.begin(115200);

	Wire.begin();
	Wire.setClock(400000);

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

	calibrate_gyroscope();

	ahrs.begin((float) SAMPLE_FREQUENCY);
	calibrate_ahrs();
}

uint32_t lastSampleMicros = 0;
uint32_t lastPrintMicros = 0;

void loop()
{
	uint32_t currentMicros = micros();

	if(currentMicros - lastSampleMicros >= MICROS_PER_SEC / SAMPLE_FREQUENCY)
	{
		read_sensors();
		update_ahrs();

		++cyclesPerSecond;

		lastSampleMicros = currentMicros;
	}

	if(currentMicros - lastPrintMicros >= MICROS_PER_SEC / PRINT_FREQUENCY)
	{
		print_info();
		lastPrintMicros = currentMicros;
	}
}
