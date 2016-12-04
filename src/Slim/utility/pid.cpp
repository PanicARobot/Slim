#include "pid.hpp"

PidController::PidController(float p, float i, float d, float min_scale, float max_scale)
	: kP(p), kI(i), kD(d),
	MIN_SCALE(min_scale), MAX_SCALE(max_scale),
	last_error(0), integral(0)
{
}

float PidController::sample(float current, float target, float delta_time)
{
	float error = target - current;

	integral += error;

	float diff = error - last_error;
	last_error = error;

	float pid = kP * error + kI * integral * delta_time + kD * diff / delta_time;

	if(pid > MAX_SCALE)
		pid = MAX_SCALE;
	else if(pid < MIN_SCALE)
		pid = MIN_SCALE;

	return pid;
}
