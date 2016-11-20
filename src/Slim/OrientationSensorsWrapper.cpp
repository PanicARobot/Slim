#include "OrientationSensorsWrapper.hpp"

#define CALIBRATION_FREQUENCY   250
#define MICROS_PER_SECOND       1000000
#define SAMPLES_COUNT           250

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
	gyro_offset = {0, 0, 0};

	for(int i = 0; i < SAMPLES_COUNT; ++i) {
		imu.read();

		gyro_offset.x += imu.g.x;
		gyro_offset.y += imu.g.y;
		gyro_offset.z += imu.g.z;

		delayMicroseconds(MICROS_PER_SECOND / CALIBRATION_FREQUENCY);
	}

	gyro_offset /= SAMPLES_COUNT;
}

void OrientationSensors::calibrate_ahrs()
{
	for(int i = 0; i < SAMPLES_COUNT; ++i) {
		update();
		delayMicroseconds(MICROS_PER_SECOND / CALIBRATION_FREQUENCY);
	}

	ahrs_offset = {0, 0, 0};

	for(int i = 0; i < SAMPLES_COUNT; ++i) {
		update();

		ahrs_offset += {
			ahrs.getRollRadians(),
			ahrs.getPitchRadians(),
			ahrs.getYawRadians()
		};
	}

	ahrs_offset /= SAMPLES_COUNT;
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
	ahrs_reading = {
		ahrs.getRollRadians(),
		ahrs.getPitchRadians(),
		ahrs.getYawRadians()
	};
	ahrs_reading -= ahrs_offset;

	// Apply accelerometer offsets
}
