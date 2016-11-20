#include "PlanarAccelerationModule.hpp"

#include <cmath>

Point3D<float> acceleration_vector;

void updatePlanarAcceleration(OrientationSensors& position) {
	acceleration_vector = {
		position.getAccX(),
		position.getAccY(),
		position.getAccZ()
	};

	float roll = position.getRoll();
	float pitch = position.getPitch();

	acceleration_vector.rotatePR(pitch, roll);
}
