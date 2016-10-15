#include "PlanarAccelerationModule.h"

#include <cmath>

#define MICROS_PER_SECOND      1000000
#define CALIBRATION_FREQUENCY  250
#define CALIBRATION_TIME       MICROS_PER_SECOND

static struct {
	double x, y, z;
} acceleration_vector;

static double gravity;

void calibrate_gravity(OrientationSensors& position) {
	acceleration_vector = {0, 0, 0};
	uint32_t samples_count = 0;

	uint8_t start_micros = micros();
	uint8_t last_micros = start_micros;

	while(true) { // time for calibration
		uint32_t current_micros = micros();

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
	acceleration_vector.x = position.getAccX(); // CHECK THESE
	acceleration_vector.y = position.getAccY(); // CHECK THESE
	acceleration_vector.z = position.getAccZ();

	// Aaand CHECK THESE
	acceleration_vector.x -= sin(position.getPitch()) * cos(position.getRoll()) * gravity;
	acceleration_vector.y += sin(position.getRoll()) * gravity;
	acceleration_vector.z -= cos(position.getPitch()) * cos(position.getRoll()) * gravity;
}
