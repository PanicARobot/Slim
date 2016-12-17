#include "fight.h"

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

static uint16_t debounce_start_timer = 0;

void initFight()
{
	debounce_start_timer = 50;

	for(int i = 0; i < 16; ++i)
	{
		enqueueTurn(STANDARD_SPEED, 0, -90);
		enqueueLinearMovement(STANDARD_SPEED, 150);
	}
}

void handleFight()
{
	if(debounce_start_timer > 0)
	{
		debounce_start_timer--;
		return;
	}

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
