#include "fight.h"

#include "../drivers/MotorDriver.h"
#include "../drivers/ProximitySensors.h"
#include "../control/Movement.h"

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
				initiateTurn(STANDARD_SPEED, 0, -90);
				STATE = SEARCH_TURNING;
			}
			break;

		case SEARCH_TURNING:
			if(IsMovementComplete())
			{
				initiateLinearMovement(STANDARD_SPEED, 200);
				STATE = SEARCH_FORWARD;
			}
			break;

		case SEARCH_FORWARD:
			if(IsMovementComplete())
			{
				initiateTurn(STANDARD_SPEED, 50, 270);
				STATE = SEARCH_TURNING;
			}
			break;

		case FOUND_LEFT:
			initiateTurn(STANDARD_SPEED, 0, -90);
			STATE = FOUND_STRAIGHT;
			break;

		case FOUND_RIGHT:
			initiateTurn(STANDARD_SPEED, 0, 90);
			STATE = FOUND_STRAIGHT;
			break;

		case FOUND_STRAIGHT:
			if(IsMovementComplete())
			{
				initiateLinearMovement(STANDARD_SPEED, 1000);
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

void handleFight(float left_speed, float right_speed)
{
	uint8_t left = readLeftSensor();
	uint8_t right = readRightSensor();

	if(left) STATE = FOUND_LEFT;
	else if(right) STATE = FOUND_RIGHT;

	MovePattern();
	handleControlledMovement(left_speed, right_speed);
}
