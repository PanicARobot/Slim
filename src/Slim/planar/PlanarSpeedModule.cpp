#include "PlanarSpeedModule.hpp"

#include "PlanarAccelerationModule.hpp"
#include "../DualEncoderDriver.hpp"

Point3D<double> speed_vector;

void initPlanarSpeed(OrientationSensors& position) {
	calibrateGravity(position);
}

void updatePlanarSpeed(OrientationSensors& position) {
	updatePlanarAcceleration(position);
	speed_vector += acceleration_vector;

	Serial.print("Acc (");
	Serial.print(acceleration_vector.x); Serial.print(", ");
	Serial.print(acceleration_vector.y); Serial.print(", ");
	Serial.print(acceleration_vector.z); Serial.print(")\t");

	Serial.print("Speed (");
	Serial.print(speed_vector.x); Serial.print(", ");
	Serial.print(speed_vector.y); Serial.print(", ");
	Serial.print(speed_vector.z); Serial.println(")");
}
