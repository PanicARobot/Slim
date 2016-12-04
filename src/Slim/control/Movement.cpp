#include "Movement.h"

#include "../drivers/MotorDriver.h"
#include "../utility/pid.hpp"

#include <math.h>

#define SPEED_CHANGE_STEP			       1
#define DISTANCE_BETWEEN_MOTORS		       85.0f
#define HALF_DISTANCE_BETWEEN_MOTORS       (DISTANCE_BETWEEN_MOTORS / 2.0f)

#define MOTOR_KP   0.5f
#define MOTOR_KI   2.0f
#define MOTOR_KD   0.0f

static bool moving = false;

static float distance_expected_by_left_tire, distance_expected_by_right_tire;

static int16_t left_target_speed, right_target_speed;

static PidController left_motor_pid(MOTOR_KP, MOTOR_KI, MOTOR_KD);
static PidController right_motor_pid(MOTOR_KP, MOTOR_KI, MOTOR_KD);

void initiateLinearMovement(int speed, int distance)
{
	// evaluate direction and pass to drivers the start state
	int direction = distance > 0 ? 1 : -1;

	// set setpoints for end of movement, used in handler
	distance_expected_by_left_tire = (float) distance;
	distance_expected_by_right_tire = (float) distance;

	// set motor speeds, used in handler
	left_target_speed = speed * direction;
	right_target_speed = speed * direction;

	left_motor_pid.zero();
	right_motor_pid.zero();

	moving = true;
}

void initiateTurn(int speed, int turn_radius, int turn_degrees)
{
	if(turn_degrees == 0) return;

	float left_radius = (float) turn_radius + HALF_DISTANCE_BETWEEN_MOTORS;
	float right_radius = (float) turn_radius - HALF_DISTANCE_BETWEEN_MOTORS;

	if(turn_degrees < 0)
	{
		float temp = left_radius;
		left_radius = right_radius;
		right_radius = temp;
	}

	distance_expected_by_left_tire = left_radius * M_PI * (float) turn_degrees / 180.0f;
	distance_expected_by_right_tire = right_radius * M_PI * (float) turn_degrees / 180.0f;

	if(turn_degrees < 0)
	{
		distance_expected_by_left_tire = -distance_expected_by_left_tire;
		distance_expected_by_right_tire = -distance_expected_by_right_tire;
	}

	left_target_speed = speed / turn_radius * left_radius;
	right_target_speed = speed / right_radius * right_radius;

	left_motor_pid.zero();
	right_motor_pid.zero();

	moving = true;
}

void handleControlledMovement(float left_tire_speed, float right_tire_speed, float delta_time)
{
	if(!moving)
	{
		// stop the motors, they should not be moving
		setMotors(0, 0);
		return;
	}

	// update moved distance
	distance_expected_by_left_tire -= (float) left_tire_speed * delta_time;
	distance_expected_by_right_tire -= (float) right_tire_speed * delta_time;

	if( (distance_expected_by_left_tire < 0 && left_target_speed > 0) ||
		(distance_expected_by_left_tire > 0 && left_target_speed < 0) )
	{
		left_target_speed = 0;
	}

	if( (distance_expected_by_right_tire < 0 && right_target_speed > 0) ||
		(distance_expected_by_right_tire > 0 && right_target_speed < 0) )
	{
		right_target_speed = 0;
	}

	if(left_target_speed == 0 && right_target_speed == 0)
	{
		resetMovement();
		return;
	}

	float left_speed = left_motor_pid.sample(left_target_speed, left_tire_speed, delta_time);
	float right_speed = right_motor_pid.sample(right_target_speed, right_tire_speed, delta_time);
	setMotors(left_speed, right_speed);
}

void resetMovement()
{
	setMotors(0, 0);
	moving = false;
}

int isMovementComplete()
{
	return !moving;
}
