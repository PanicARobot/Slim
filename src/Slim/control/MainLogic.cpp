#include "MainLogic.h"

#include "../drivers/MotorDriver.h"
#include "../drivers/DualEncoderDriver.hpp"
#include "../drivers/ProximitySensors.hpp"
#include "../modules/FrontLiftedDetection.hpp"
#include "Movement.h"

#define STANDARD_SPEED				       170

enum {
	SEARCH_INITIAL,
	SEARCH_TURNING,
	SEARCH_FORWARD,

	FOUND_LEFT,
	FOUND_RIGHT,
	FOUND_STRAIGHT,

	STOP_MOVING
} STATE = SEARCH_INITIAL;


void MovePattern()
{
	switch(STATE)
	{
		case SEARCH_INITIAL:
			if(IsMovementComplete())
			{
				Turn(0, -90);
				STATE = SEARCH_TURNING;
			}
			break;

		case SEARCH_TURNING:
			if(IsMovementComplete())
			{
				LinearMovement(200);
				STATE = SEARCH_FORWARD;
			}
			break;

		case SEARCH_FORWARD:
			if(IsMovementComplete())
			{
				Turn(50, 270);
				STATE = SEARCH_TURNING;
			}
			break;

		case FOUND_LEFT:
			Turn(0, -90);
			STATE = FOUND_STRAIGHT;
			break;

		case FOUND_RIGHT:
			Turn(0, 90);
			STATE = FOUND_STRAIGHT;
			break;

		case FOUND_STRAIGHT:
			if(IsMovementComplete())
			{
				LinearMovement(1000);
				STATE = FOUND_STRAIGHT;
			}
			break;

		default:
			if(IsMovementComplete())
			{
				setMotors(0, 0);
			}
			break;
	}
}

/*
void RoundPattern()
{
	switch(STATE)
	{
		case INITIAL_STATE:
			{
				Turn(200,360);
				STATE = STATE_TURNING;
			}
			break;

		case STATE_TURNING:
			if(IsMovementComplete())
			{
				// keep turning
				Turn(200,360);
			}
			break;

		default:
			setMotors(0, 0);
			break;
	}
	// if( digitalRead(LEFT_SENSOR_PIN) ^ 1 == 1 || digitalRead(RIGHT_SENSOR_PIN) ^ 1 == 1)
	// {
	// 	STATE = EXIT_STATE_ON_SIDE;
	// }
	
	// if(OutOfRing())
	// {
	// 	STATE = EXIT_STATE_OUT_OF_RING;
	// }

	// if(Impact())
	// {
	// 	STATE = EXIT_STATE_ON_IMPACT;
	// }
}
*/

void MainLogic(float left_speed, float right_speed)
{
	uint8_t left, right;
	readProximitySensors(left, right);

	if(left) STATE = FOUND_LEFT;
	else if(right) STATE = FOUND_RIGHT;

	MovePattern();
	handleControlledMovement(left_speed, right_speed);
}
