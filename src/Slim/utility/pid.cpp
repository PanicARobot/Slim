#include "pid.hpp"

PidController::PidController(float p, float i, float d, float flank)
	: kP(p), kI(i), kD(d),
	last_error(0), integral(0),
	integral_flank(flank)
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

	integral += error * delta_time;
	if(integral > integral_flank)
		integral = integral_flank;
	else if(integral < -integral_flank)
		integral = -integral_flank;

	float derivative = (error - last_error) / delta_time;

	last_error = error;

	return kP * error + kI * integral + kD * derivative;
}
