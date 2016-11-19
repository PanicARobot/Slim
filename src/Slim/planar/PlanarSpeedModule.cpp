#include "PlanarSpeedModule.hpp"

#include "PlanarAccelerationModule.hpp"
#include "../DualEncoderDriver.hpp"

static Point3D<double> speed_vector;

void initPlanarSpeed(OrientationSensors& position) {
	calibrateGravity(position);
}

void updatePlanarSpeed(OrientationSensors& position) {
	updatePlanarAcceleration(position);
	speed_vector += acceleration_vector;
}
