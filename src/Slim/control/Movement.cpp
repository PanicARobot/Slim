#include "Movement.h"

#include "../drivers/MotorDriver.h"
#include "../utility/pid.hpp"

#include <math.h>

#define SPEED_CHANGE_STEP			       1
#define DISTANCE_BETWEEN_MOTORS		       85.0f
#define HALF_DISTANCE_BETWEEN_MOTORS       (DISTANCE_BETWEEN_MOTORS / 2.0f)

#define MOTOR_KP   0.05f
#define MOTOR_KI   6.0f
#define MOTOR_KD   0.0f

#define QUEUE_SIZE 64
#define QUEUE_FRONT movement_queue[left_index]
#define QUEUE_BACK movement_queue[right_index]

static PidController left_motor_pid(MOTOR_KP, MOTOR_KI, MOTOR_KD, 255.0f / MOTOR_KI);
static PidController right_motor_pid(MOTOR_KP, MOTOR_KI, MOTOR_KD, 255.0f / MOTOR_KI);

struct Movement {
	float expected_distance_left;
	float expected_distance_right;

	int16_t left_target_speed;
	int16_t right_target_speed;
};

static Movement movement_queue[QUEUE_SIZE];
int left_index = 0, right_index = 0;

inline void push_queue()
{
	++right_index;
	if(right_index == QUEUE_SIZE)
		right_index = 0;
}

inline void pop_queue()
{
	++left_index;
	if(left_index == QUEUE_SIZE)
		left_index = 0;
}

void enqueueLinearMovement(int speed, int distance)
{
	// evaluate direction and pass to drivers the start state
	int direction = distance > 0 ? 1 : -1;

	// set setpoints for end of movement, used in handler
	QUEUE_BACK.expected_distance_left = (float) distance;
	QUEUE_BACK.expected_distance_right = (float) distance;

	// set motor speeds, used in handler
	QUEUE_BACK.left_target_speed = speed * direction;
	QUEUE_BACK.right_target_speed = speed * direction;

	push_queue();
}

void enqueueTurn(int speed, int turn_radius, int turn_degrees)
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

	QUEUE_BACK.expected_distance_left = left_radius * M_PI * (float) turn_degrees / 180.0f;
	QUEUE_BACK.expected_distance_right = right_radius * M_PI * (float) turn_degrees / 180.0f;

	if(turn_degrees < 0)
	{
		QUEUE_BACK.expected_distance_left = -QUEUE_BACK.expected_distance_left;
		QUEUE_BACK.expected_distance_right = -QUEUE_BACK.expected_distance_right;
	}

	if(turn_degrees > 0)
	{
		QUEUE_BACK.left_target_speed = QUEUE_BACK.expected_distance_left > 0 ? speed : -speed;
		QUEUE_BACK.right_target_speed = QUEUE_BACK.left_target_speed / left_radius * right_radius;
	}
	else
	{
		QUEUE_BACK.right_target_speed = QUEUE_BACK.expected_distance_right > 0 ? speed : -speed;
		QUEUE_BACK.left_target_speed = QUEUE_BACK.right_target_speed / right_radius * left_radius;
	}

	push_queue();
}

void handleControlledMovement(float left_tire_speed, float right_tire_speed, float delta_time)
{
	if(left_index == right_index)
	{
		// stop the motors, they should not be moving
		setMotors(0, 0);
		return;
	}

	// update moved distance
	QUEUE_FRONT.expected_distance_left -= (float) left_tire_speed * delta_time;
	QUEUE_FRONT.expected_distance_right -= (float) right_tire_speed * delta_time;

	float left_speed = left_motor_pid.sample(QUEUE_FRONT.left_target_speed, left_tire_speed, delta_time);
	float right_speed = right_motor_pid.sample(QUEUE_FRONT.right_target_speed, right_tire_speed, delta_time);

	setMotors(left_speed, right_speed);

	if( (QUEUE_FRONT.expected_distance_left < 0 && QUEUE_FRONT.left_target_speed > 0) ||
		(QUEUE_FRONT.expected_distance_left > 0 && QUEUE_FRONT.left_target_speed < 0) ||
		(QUEUE_FRONT.expected_distance_right < 0 && QUEUE_FRONT.right_target_speed > 0) ||
		(QUEUE_FRONT.expected_distance_right > 0 && QUEUE_FRONT.right_target_speed < 0) )
	{
		pop_queue();
	}

	if(left_index == right_index)
	{
		setMotors(0, 0);
		left_motor_pid.zero();
		right_motor_pid.zero();
	}
}

int isQueueEmpty()
{
	return left_index == right_index;
}

void clearQueue()
{
	setMotors(0, 0);
	left_motor_pid.zero();
	right_motor_pid.zero();
	right_index = left_index;
}
