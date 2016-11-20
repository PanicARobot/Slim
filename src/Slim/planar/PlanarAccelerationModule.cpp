#include "PlanarAccelerationModule.hpp"

#include <cmath>

#define MICROS_PER_SECOND      1000000
#define CALIBRATION_FREQUENCY  250
#define CALIBRATION_TIME       MICROS_PER_SECOND

Point3D<double> acceleration_vector;

static double gravity;

void calibrateGravity(OrientationSensors& position) {
	acceleration_vector = {0, 0, 0};
	uint32_t samples_count = 0;

	uint8_t start_micros = micros();
	uint8_t last_micros = start_micros;

		acceleration_vector += {
			position.getAccX(),
			position.getAccY(),
			position.getAccZ()
		};

		if(current_micros - start_micros > CALIBRATION_TIME)
			break;

		if(current_micros - last_micros < MICROS_PER_SECOND / CALIBRATION_FREQUENCY)
			continue;

		acceleration_vector.x += position.getAccX(); // CHECK THESE
		acceleration_vector.y += position.getAccY(); // CHECK THESE
		acceleration_vector.z += position.getAccZ();
	}

	acceleration_vector.x /= samples_count;
	acceleration_vector.y /= samples_count;
	acceleration_vector.z /= samples_count;

	gravity = sqrt(acceleration_vector.x * acceleration_vector.x
			+ acceleration_vector.y * acceleration_vector.y
			+ acceleration_vector.z * acceleration_vector.z);
}

void updatePlanarAcceleration(OrientationSensors& position) {
	/*
	   acceleration_vector = {
	   position.getAccX(),
	   position.getAccY(),
	   position.getAccZ()
	   };

	   float roll = position.getRoll();
	   float pitch = position.getPitch();

	   acceleration_vector = {
	   cos(pitch)  * acceleration_vector.x + sin(pitch) * sin(roll) * acceleration_vector.y + sin(pitch) * cos(roll) * acceleration_vector.z,
	   0           * acceleration_vector.x +              cos(roll) * acceleration_vector.y -              sin(roll) * acceleration_vector.z,
	   -sin(pitch) * acceleration_vector.x + cos(pitch) * sin(roll) * acceleration_vector.y + cos(pitch) * cos(roll) * acceleration_vector.z
	   };
	   */

	acceleration_vector = {
		position.getAccX(),
		position.getAccY(),
		position.getAccZ()
	};

	float roll = -position.getRoll();
	float pitch = -position.getPitch();

	/*
	acceleration_vector -= {
		+ sin(pitch) * cos(roll) * gravity,
		-              sin(roll) * gravity,
		+ cos(pitch) * cos(roll) * gravity
	};
	*/
	acceleration_vector -= {
		             sin(pitch) * gravity,
		-sin(roll) * cos(pitch) * gravity,
		cos(roll)  * cos(pitch) * gravity
	};
}
