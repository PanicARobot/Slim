#include "PlanarAccelerationModule.h"

#include "DualEncoderDriver.h"

#include "constants.h"

void PlanarAcceleration::update()
{
	static constexpr float d = 85; // mm

	float vl = leftEncoder.getSpeed();
	float al = leftEncoder.getAcc();

	float vr = rightEncoder.getSpeed();
	float ar = rightEncoder.getAcc();

	acc_x = (vl * vl - vr * vr) / (4 * d);
	acc_y = (al + ar) / 2;
}

PlanarAcceleration planarAcceleration;
