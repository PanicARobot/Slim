#include "Movement.h"

#include "../drivers/MotorDriver.h"
#include "../utility/pid.hpp"

#include <math.h>

#define SPEED_CHANGE_STEP			       1
#define DISTANCE_BETWEEN_MOTORS		       85.00
#define HALF_DISTANCE_BETWEEN_MOTORS       (DISTANCE_BETWEEN_MOTORS / 2.00)

#define MOTOR_KP   1.0f
#define MOTOR_KI   0.2f
#define MOTOR_KD   2.0f

static bool moving = false;

static float distance_expected_by_left_tire, distance_expected_by_right_tire;

static int16_t left_target_speed, right_target_speed, left_motor_dir, right_motor_dir;
static int8_t direction_of_linear_movement;
static float circle_motor_speed_difference;

static PidController left_motor_pid(MOTOR_KP, MOTOR_KI, MOTOR_KD);
static PidController right_motor_pid(MOTOR_KP, MOTOR_KI, MOTOR_KD);

void initiateLinearMovement(int speed, int distance)
{
	// evaluate direction and pass to drivers the start state
	direction_of_linear_movement = distance > 0 ? 1 : -1;

	// set setpoints for end of movement, used in handler
	distance_expected_by_left_tire = (float) distance;
	distance_expected_by_right_tire = (float) distance;

	// set motor speeds, used in handler
	left_target_speed = speed * direction_of_linear_movement;
	right_target_speed = speed * direction_of_linear_movement;

	left_motor_pid.zero();
	right_motor_pid.zero();

	moving = true;
}

void initiateTurn(int speed, int turn_radius, int turn_degrees)
{
	if(turn_degrees == 0) return;

	distance_expected_by_left_tire  = 2.00 * ((float)turn_radius + HALF_DISTANCE_BETWEEN_MOTORS) * M_PI * (float)turn_degrees / 360;
	distance_expected_by_right_tire = 2.00 * ((float)turn_radius - HALF_DISTANCE_BETWEEN_MOTORS) * M_PI * (float)turn_degrees / 360;
	if(turn_degrees < 0)
	{
		float temp = -distance_expected_by_left_tire;
		distance_expected_by_left_tire = -distance_expected_by_right_tire;
		distance_expected_by_right_tire = temp;
	}

	circle_motor_speed_difference = distance_expected_by_left_tire / distance_expected_by_right_tire;

	left_motor_dir = (turn_degrees > 0 || distance_expected_by_left_tire > 0) ? 1 : -1;
	right_motor_dir = (turn_degrees < 0 || distance_expected_by_right_tire > 0) ? 1 : -1;

	left_target_speed = speed * left_motor_dir;
	right_target_speed = speed * right_motor_dir;

	if(turn_degrees > 0) left_target_speed *= speed;
	else right_target_speed *= speed;

	setMotors(left_target_speed, right_target_speed);

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

	if( (distance_expected_by_left_tire < 0 && direction_of_linear_movement > 0) ||
			(distance_expected_by_left_tire > 0 && direction_of_linear_movement < 0) )
	{
		left_target_speed = 0;
	}

	if( (distance_expected_by_right_tire < 0 && direction_of_linear_movement > 0) ||
			(distance_expected_by_right_tire > 0 && direction_of_linear_movement < 0) )
	{
		right_target_speed = 0;
	}

	if(left_target_speed == 0 && right_target_speed == 0)
	{
		moving = false;
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
