#include "PlanarAccelerationModule.h"

#include "OrientationSensorsWrapper.h"
#include "DualEncoderDriver.h"
#include "MotorDriver.h"

#include "constants.h"

void PlanarAcceleration::calibrate()
{
	acc_offset_x = 0;
	acc_offset_y = 0;

	int samples_count = 0;

	uint32_t start_micros = micros();
	uint32_t last_micros;

	float last_left_speed = 0;
	float last_right_speed = 0;

	setMotors(MOTORS_CALIBRATE_SPEED);

	while(true)
	{
		uint32_t current_micros = micros();
		if(current_micros - start_micros > MICROS_PER_SECOND / 3)
			return;

		if(current_micros - last_micros < MICROS_PER_SECOND / CALIBRATION_FREQUENCY)
			continue;

		float left_speed = leftEncoder.getSpeed();
		float right_speed = rightEncoder.getSpeed();

		float acceleration = (left_speed + right_speed - last_left_speed - last_right_speed) / 2;

		acc_offset_x += position.getAccX();
		acc_offset_y += position.getAccY() - acceleration;

		++samples_count;

		last_left_speed = left_speed;
		last_right_speed = right_speed;
	}

	acc_offset_x /= samples_count;
	acc_offset_y /= samples_count;

	Serial.print(acc_offset_x); Serial.print(" ");
	Serial.print(acc_offset_y); Serial.println("");

	setMotors(0, 0);
}
