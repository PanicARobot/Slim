#ifndef __ORIENTATION_SENSORS_WRAPPER_H
#define __ORIENTATION_SENSORS_WRAPPER_H

#include "utility/Point3D.hpp"

#include <LSM6.h>
#include <MahonyAHRS.h>

#include <cstdint>

class OrientationSensors {
	private:
		static constexpr float gScale = 0x7FFF / (float)8;
		static constexpr float dpsScale = 0x7FFF / (float)1000;

		LSM6 imu;
		Mahony ahrs;

		Point3D<int32_t> acc_offset;
		Point3D<int32_t> gyro_offset;

		Point3D<float> ahrs_offset;
		Point3D<float> ahrs_reading;

		void calibrate_sensors();
		void calibrate_ahrs();

		void update_ahrs();

	public:
		void init();
		void calibrate();
		void update();

		inline float getAccX() { return imu.a.x / gScale; }
		inline float getAccY() { return imu.a.y / gScale; }
		inline float getAccZ() { return imu.a.z / gScale; }

		inline float getGyroX() { return imu.g.x / dpsScale; }
		inline float getGyroY() { return imu.g.y / dpsScale; }
		inline float getGyroZ() { return imu.g.z / dpsScale; }

		inline float getRoll() { return ahrs_reading.x; }
		inline float getPitch() { return ahrs_reading.y; }
		inline float getYaw() { return ahrs_reading.z; }
};

#endif // __ORIENTATION_SENSORS_WRAPPER_H
