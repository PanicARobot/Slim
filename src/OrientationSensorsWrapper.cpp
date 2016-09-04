#include "OrientationSensorsWrapper.h"

const int MICROS_PER_SEC = 1000000;
const int SAMPLE_FREQUENCY = 250;

OrientationSensors::OrientationSensors()
{
	// do not leave uninitialized variables
	gyro_offset = {0, 0, 0};
	ahrs_offset = {0, 0, 0};
	ahrs_reading = {0, 0, 0};
}

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
}

void OrientationSensors::calibrate()
{
	calibrate_gyroscope();

	ahrs.begin((float) SAMPLE_FREQUENCY);
	calibrate_ahrs();
}

void OrientationSensors::calibrate_gyroscope()
{
	int samples_count = 0;

	xyz<int64_t> gyro_sum {0, 0, 0};

	// Collect sensor data for a second
	uint32_t startMicros = micros();
	while(micros() - startMicros < MICROS_PER_SEC)
	{
		imu.read();

		gyro_sum.x += imu.g.x;
		gyro_sum.y += imu.g.y;
		gyro_sum.z += imu.g.z;

		++samples_count;
	}

	gyro_offset.x = gyro_sum.x / samples_count;
	gyro_offset.y = gyro_sum.y / samples_count;
	gyro_offset.z = gyro_sum.z / samples_count;
}

void OrientationSensors::calibrate_ahrs()
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
	}

	int samples_count = 0;

	xyz<float> ahrs_sum {0, 0, 0};

	// Collect data for a second
	startMicros = micros();
	lastSampleMicros = startMicros - MICROS_PER_SEC / SAMPLE_FREQUENCY;

	while(true)
	{
		uint32_t currentMicros = micros();
		if(currentMicros - startMicros > MICROS_PER_SEC)
			break;

		if(currentMicros - lastSampleMicros < MICROS_PER_SEC / SAMPLE_FREQUENCY)
			continue;

		lastSampleMicros = currentMicros;

		read_sensors();

		ahrs_sum.x += ahrs.getRollRadians();
		ahrs_sum.y += ahrs.getPitchRadians();
		ahrs_sum.z += ahrs.getYawRadians();

		++samples_count;
	}

	ahrs_offset.x = ahrs_sum.x / samples_count;
	ahrs_offset.y = ahrs_sum.y / samples_count;
	ahrs_offset.z = ahrs_sum.z / samples_count;
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
