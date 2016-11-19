#include "PlanarAccelerationModule.hpp"

#include <cmath>

#define MICROS_PER_SECOND      1000000
#define CALIBRATION_FREQUENCY  250
#define SAMPLES_COUNT          250

Point3D<float> acceleration_vector;

static float gravity;

void calibrateGravity(OrientationSensors& position) {
	acceleration_vector = {0, 0, 0};

	for(int i = 0; i < SAMPLES_COUNT; ++i) {
		position.update();

		acceleration_vector += {
			position.getAccX(), // CHECK THESE
			position.getAccY(), // CHECK THESE
			position.getAccZ()
		};

		delayMicroseconds(MICROS_PER_SECOND / CALIBRATION_FREQUENCY);
	}

	acceleration_vector /= SAMPLES_COUNT;

	gravity = sqrt(acceleration_vector.x * acceleration_vector.x
			+ acceleration_vector.y * acceleration_vector.y
			+ acceleration_vector.z * acceleration_vector.z);
}

void updatePlanarAcceleration(OrientationSensors& position) {

	acceleration_vector.x = position.getAccX(); // CHECK THESE
	acceleration_vector.y = position.getAccY(); // CHECK THESE
	acceleration_vector.z = position.getAccZ();

	// Aaand CHECK THESE
	acceleration_vector.x -= sin(position.getPitch()) * cos(position.getRoll()) * gravity;
	acceleration_vector.y += sin(position.getRoll()) * gravity;
	acceleration_vector.z -= cos(position.getPitch()) * cos(position.getRoll()) * gravity;
}
