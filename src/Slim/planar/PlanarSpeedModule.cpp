#include "PlanarSpeedModule.hpp"

#include "PlanarAccelerationModule.hpp"
#include "../DualEncoderDriver.hpp"

Point3D<float> speed_vector;

void initPlanarSpeed() {
	speed_vector = {0, 0, 0};
}

void updatePlanarSpeed(OrientationSensors& position) {
	updatePlanarAcceleration(position);
	speed_vector += acceleration_vector;
}
