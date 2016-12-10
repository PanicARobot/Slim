#include "fight.h"

#include "../drivers/MotorDriver.h"
#include "../drivers/ProximitySensors.h"
#include "../control/Movement.h"

#define STANDARD_SPEED				       1000

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
			if(isQueueEmpty())
			{
				enqueueTurn(STANDARD_SPEED, 0, -90);
				STATE = SEARCH_TURNING;
			}
			break;

		case SEARCH_TURNING:
			if(isQueueEmpty())
			{
				enqueueLinearMovement(STANDARD_SPEED, 200);
				STATE = SEARCH_FORWARD;
			}
			break;

		case SEARCH_FORWARD:
			if(isQueueEmpty())
			{
				enqueueTurn(STANDARD_SPEED, 50, 270);
				STATE = SEARCH_TURNING;
			}
			break;

		case FOUND_LEFT:
			enqueueTurn(STANDARD_SPEED, 0, -90);
			STATE = FOUND_STRAIGHT;
			break;

		case FOUND_RIGHT:
			enqueueTurn(STANDARD_SPEED, 0, 90);
			STATE = FOUND_STRAIGHT;
			break;

		case FOUND_STRAIGHT:
			if(isQueueEmpty())
			{
				enqueueLinearMovement(STANDARD_SPEED, 1000);
				STATE = FOUND_STRAIGHT;
			}
			break;

		default:
			if(isQueueEmpty())
			{
				setMotors(0, 0);
			}
			break;
	}
}

void initFight()
{
}

void handleFight(void)
{
	uint8_t left = readLeftSensor();
	uint8_t right = readRightSensor();

	if(left) STATE = FOUND_LEFT;
	else if(right) STATE = FOUND_RIGHT;

	MovePattern();
}
