#include "pid.hpp"

PidController::PidController(float p, float i, float d)
	: kP(p), kI(i), kD(d),
	last_error(0), integral(0)
{
}

void PidController::zero()
{
	last_error = 0;
	integral = 0;
}

float PidController::sample(float target, float current, float delta_time)
{
	float error = target - current;

	integral += error;

	float diff = error - last_error;
	last_error = error;

	return kP * error + kI * integral * delta_time + kD * diff / delta_time;
}
