#include "OrientationSensorsWrapper.h"

#define CALIBRATION_FREQUENCY   250
#define MICROS_PER_SECOND       1000000

void OrientationSensors::init()
{
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

	// TODO: read calibration settings from EEPROM
}

void OrientationSensors::calibrate()
{
	calibrate_sensors();

	ahrs.begin((float) CALIBRATION_FREQUENCY);
	calibrate_ahrs();
}

void OrientationSensors::calibrate_sensors()
{
	int samples_count = 0;

	acc_offset = {0, 0, 0};
	gyro_offset = {0, 0, 0};

	// Collect sensor data for a second
	uint32_t start_micros = micros();
	uint32_t last_micros = start_micros - MICROS_PER_SECOND;

	while(true)
	{
		uint32_t current_micros = micros();
		if(current_micros - start_micros > MICROS_PER_SECOND)
			break;

		if(current_micros - last_micros < MICROS_PER_SECOND / CALIBRATION_FREQUENCY)
			continue;

		imu.read();

		acc_offset.x += imu.a.x;
		acc_offset.y += imu.a.y;
//		acc_offset.z += imu.a.z; let gravity stay there

		gyro_offset.x += imu.g.x;
		gyro_offset.y += imu.g.y;
		gyro_offset.z += imu.g.z;

		++samples_count;
	}

	acc_offset.x /= samples_count;
	acc_offset.y /= samples_count;
	acc_offset.z /= samples_count;

	gyro_offset.x /= samples_count;
	gyro_offset.y /= samples_count;
	gyro_offset.z /= samples_count;
}

void OrientationSensors::calibrate_ahrs()
{
	uint32_t start_micros = micros();
	uint32_t last_micros = start_micros - MICROS_PER_SECOND;

	// Give ahrs a second
	while(true)
	{
		uint32_t current_micros = micros();
		if(current_micros - start_micros > MICROS_PER_SECOND)
			break;

		if(current_micros - last_micros < MICROS_PER_SECOND / CALIBRATION_FREQUENCY)
			continue;

		last_micros = current_micros;

		update();
	}

	int samples_count = 0;
	ahrs_offset = {0, 0, 0};

	// Collect data for a second
	start_micros = micros();
	last_micros = start_micros - MICROS_PER_SECOND;

	while(true)
	{
		uint32_t current_micros = micros();
		if(current_micros - start_micros > MICROS_PER_SECOND)
			break;

		if(current_micros - last_micros < MICROS_PER_SECOND / CALIBRATION_FREQUENCY)
			continue;

		last_micros = current_micros;

		update();

		ahrs_offset.x += ahrs.getRollRadians();
		ahrs_offset.y += ahrs.getPitchRadians();
		ahrs_offset.z += ahrs.getYawRadians();

		++samples_count;
	}

	ahrs_offset.x /= samples_count;
	ahrs_offset.y /= samples_count;
	ahrs_offset.z /= samples_count;
}

void OrientationSensors::update_ahrs()
{
	ahrs.updateIMU(getGyroX(), getGyroY(), getGyroZ(),
			getAccX(), getAccY(), getAccZ());
}

void OrientationSensors::update()
{
	imu.read();

	// Apply gyroscope offsets
	imu.g.x -= gyro_offset.x;
	imu.g.y -= gyro_offset.y;
	imu.g.z -= gyro_offset.z;

	update_ahrs();

	// Apply ahrs offsets
	ahrs_reading.x = ahrs.getRollRadians() - ahrs_offset.x;
	ahrs_reading.y = ahrs.getPitchRadians() - ahrs_offset.y;
	ahrs_reading.z = ahrs.getYawRadians() - ahrs_offset.z;

	// Apply accelerometer offsets
	imu.a.x -= acc_offset.x;
	imu.a.y -= acc_offset.y;
	imu.a.z -= acc_offset.z;
}

OrientationSensors position;
