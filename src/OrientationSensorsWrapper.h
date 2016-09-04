#ifndef __ORIENTATION_SENSORS_WRAPPER_H
#define __ORIENTATION_SENSORS_WRAPPER_H

#include <LSM6.h>
#include <MahonyAHRS.h>

class OrientationSensors {
	private:
		static constexpr float gScale = 0x7FFF / (float)8;
		static constexpr float dpsScale = 0x7FFF / (float)1000;

		LSM6 imu;
		Mahony ahrs;

		template<typename _T>
			struct xyz {
				_T x, y, z;
		};

		xyz<int> gyro_offset;
		xyz<float> ahrs_offset;
		xyz<float> ahrs_reading;

		void calibrate_gyroscope();
		void calibrate_ahrs();

		void update_ahrs();

	public:
		OrientationSensors();

		void init();
		void calibrate();
		void read_sensors();

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
