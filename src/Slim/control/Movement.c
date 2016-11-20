#include "Movement.h"

#include "../drivers/MotorDriver.h"

#include <math.h>

#define SPEED_CHANGE_STEP			       1
#define DISTANCE_BETWEEN_MOTORS		       85.00
#define HALF_DISTANCE_BETWEEN_MOTORS       (DISTANCE_BETWEEN_MOTORS / 2.00)

#define SAMPLE_FREQUENCY        208

enum {
	NO_MOVEMENT,
	LINEAR_MOVEMENT,
	ROUND_MOVEMENT
} movement_system_status = NO_MOVEMENT;

static float distance_expected_by_left_tire, distance_expected_by_right_tire;

static int16_t left_motor_speed, right_motor_speed, left_motor_dir, right_motor_dir;
static int8_t direction_of_linear_movement;
static float circle_motor_speed_difference;
static int movement_speed;

void initiateLinearMovement(int speed, int distance) // TODO: Fix backwards
{
	movement_speed = speed;

	// evaluate direction and pass to drivers the start state
	if(0 < distance)
	{
		direction_of_linear_movement = 1;
		setMotors(movement_speed, movement_speed);
	}
	else
	{
		direction_of_linear_movement = -1;
		setMotors(-movement_speed, -movement_speed);
	}

	// set setpoints for end of movement, used in handler
	distance_expected_by_left_tire = (float) distance;
	distance_expected_by_right_tire = (float) distance;

	// set motor speeds, used in handler
	left_motor_speed = movement_speed;
	right_motor_speed = movement_speed;

	// set state on state machine
	movement_system_status = LINEAR_MOVEMENT;
}

void initiateTurn(int speed, int turn_radius, int turn_degrees)
{
	if(turn_degrees == 0) return;

	movement_speed = speed;

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

	left_motor_speed = movement_speed * left_motor_dir;
	right_motor_speed = movement_speed * right_motor_dir;

	if(turn_degrees > 0) left_motor_speed *= movement_speed;
	else right_motor_speed *= movement_speed;

	setMotors(left_motor_speed, right_motor_speed);

	// set state on state machine
	movement_system_status = ROUND_MOVEMENT;
}

void handleControlledMovement(float left_tire_speed, float right_tire_speed)
{
	switch(movement_system_status)
	{
		case NO_MOVEMENT:
			// stop the motors, they should not be moving
			setMotors(0, 0);
			break;

		case LINEAR_MOVEMENT:
		{
			// check if the speeds are the same if not correct the speed of the slower motors
			if(left_tire_speed > right_tire_speed)
			{
				left_motor_speed -= SPEED_CHANGE_STEP;
			}
			else if(left_tire_speed < right_tire_speed)
			{
				left_motor_speed += SPEED_CHANGE_STEP;
			}

			// update moved distance
			distance_expected_by_left_tire -= (float)(left_tire_speed / SAMPLE_FREQUENCY);
			distance_expected_by_right_tire -= (float)(right_tire_speed / SAMPLE_FREQUENCY);

			if(	(distance_expected_by_left_tire < 0 && direction_of_linear_movement > 0) ||
				(distance_expected_by_left_tire > 0 && direction_of_linear_movement < 0) )
			{
				left_motor_speed = 0;
			}


			if(	(distance_expected_by_right_tire < 0 && direction_of_linear_movement > 0) ||
				(distance_expected_by_right_tire > 0 && direction_of_linear_movement < 0) )
			{
				right_motor_speed = 0;
			}

			if(left_motor_speed == 0 && right_motor_speed == 0)
			{
				movement_system_status = NO_MOVEMENT;
			}

			setMotors(left_motor_speed, right_motor_speed);

			break;
		}

		case ROUND_MOVEMENT:
		{
			// check if the speeds are the same if not correct the speed of the slower motors
			if(left_tire_speed / right_tire_speed > circle_motor_speed_difference)
			{
				left_motor_speed -= SPEED_CHANGE_STEP;
				right_motor_speed += SPEED_CHANGE_STEP;
			}
			else if(left_tire_speed / right_tire_speed < circle_motor_speed_difference)
			{
				left_motor_speed += SPEED_CHANGE_STEP;
				right_motor_speed -= SPEED_CHANGE_STEP;
			}

			// update moved distance
			distance_expected_by_left_tire -= left_tire_speed / SAMPLE_FREQUENCY;
			distance_expected_by_right_tire -= right_tire_speed / SAMPLE_FREQUENCY;

			if(	(distance_expected_by_left_tire < 0 && left_motor_dir > 0) ||
				(distance_expected_by_left_tire > 0 && left_motor_dir < 0) )
			{
				left_motor_speed = 0;
			}

			if(	(distance_expected_by_right_tire < 0 && right_motor_dir > 0) ||
				(distance_expected_by_right_tire > 0 && right_motor_dir < 0) )
			{
				right_motor_speed = 0;
			}

			if(left_motor_speed == 0 && right_motor_speed == 0)
			{
				movement_system_status = NO_MOVEMENT;
			}

			setMotors(left_motor_speed, right_motor_speed);

			break;
		}
	}
}

int IsMovementComplete(void)
{
	return movement_system_status == NO_MOVEMENT;
}
