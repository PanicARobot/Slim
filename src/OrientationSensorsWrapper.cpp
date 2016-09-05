#include "OrientationSensorsWrapper.h"

const int MICROS_PER_SEC = 1000000;
const int CALIBRATION_FREQUENCY = 250;

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
	calibrate_gyroscope(); // Remove gyro jitters

	ahrs.begin((float) CALIBRATION_FREQUENCY);
	calibrate_ahrs();
}

void OrientationSensors::calibrate_gyroscope()
{
	int samples_count = 0;
	gyro_offset = {0, 0, 0};

	// Collect sensor data for a second
	uint32_t start_micros = micros();
	uint32_t last_micros = start_micros - MICROS_PER_SEC;

	while(true)
	{
		uint32_t current_micros = micros();
		if(current_micros - start_micros > MICROS_PER_SEC)
			break;

		if(current_micros - last_micros < MICROS_PER_SEC / CALIBRATION_FREQUENCY)
			continue;

		imu.read();

		gyro_offset.x += imu.g.x;
		gyro_offset.y += imu.g.y;
		gyro_offset.z += imu.g.z;

		++samples_count;
	}

	gyro_offset.x /= samples_count;
	gyro_offset.y /= samples_count;
	gyro_offset.z /= samples_count;
}

void OrientationSensors::calibrate_ahrs()
{
	uint32_t start_micros = micros();
	uint32_t last_micros = start_micros - MICROS_PER_SEC;

	// Give ahrs a second
	while(true)
	{
		uint32_t current_micros = micros();
		if(current_micros - start_micros > MICROS_PER_SEC)
			break;

		if(current_micros - last_micros < MICROS_PER_SEC / CALIBRATION_FREQUENCY)
			continue;

		last_micros = current_micros;

		read_sensors();
	}

	int samples_count = 0;
	ahrs_offset = {0, 0, 0};

	// Collect data for a second
	start_micros = micros();
	last_micros = start_micros - MICROS_PER_SEC;

	while(true)
	{
		uint32_t current_micros = micros();
		if(current_micros - start_micros > MICROS_PER_SEC)
			break;

		if(current_micros - last_micros < MICROS_PER_SEC / CALIBRATION_FREQUENCY)
			continue;

		last_micros = current_micros;

		read_sensors();

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

void OrientationSensors::read_sensors()
{
	imu.read();

	// Apply gyroscope offsets
	imu.g.x -= gyro_offset.x;
	imu.g.y -= gyro_offset.y;
	imu.g.z -= gyro_offset.z;

	update_ahrs();

	ahrs_reading.x = ahrs.getRollRadians() - ahrs_offset.x;
	ahrs_reading.y = ahrs.getPitchRadians() - ahrs_offset.y;
	ahrs_reading.z = ahrs.getYawRadians() - ahrs_offset.z;
}

OrientationSensors position;
