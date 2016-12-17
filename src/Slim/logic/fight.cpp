#include "fight.h"

#include "../drivers/MotorDriver.h"
#include "../drivers/ProximitySensors.h"
#include "../control/Movement.h"

#define STANDARD_SPEED   800
#define TURN_SPEED       1000
#define PUSH_SPEED       3000

enum {
	SEARCH,
	FOUND,
	CHASE,
} STATE = SEARCH;

void initFight()
{
	enqueueTurn(STANDARD_SPEED, 0, -20);
	enqueueLinearMovement(STANDARD_SPEED, 250);
	enqueueTurn(STANDARD_SPEED, 75, -360);
	enqueueTurn(STANDARD_SPEED, 75, -360);
	enqueueTurn(STANDARD_SPEED, 75, -360);
	enqueueTurn(STANDARD_SPEED, 75, -360);
}

void handleFight()
{
	uint8_t left = readLeftSensor();
	uint8_t right = readRightSensor();

	if(STATE != FOUND)
	{
		if(left)
		{
			STATE = FOUND;
			clearQueue();
			enqueueTurn(TURN_SPEED, 0, -90);
		}
		else if(right)
		{
			STATE = FOUND;
			clearQueue();
			enqueueTurn(TURN_SPEED, 0, 90);
		}
	}
	else
	{
		if(isQueueEmpty())
		{
			enqueueLinearMovement(PUSH_SPEED, 1000);
			STATE = CHASE;
		}
	}
}
