#include "test.h"

#include "../drivers/MotorDriver.h"
#include "../drivers/ProximitySensors.h"
#include "../control/Movement.h"

#include <Arduino.h>

#define SPEED 1000

void initTest()
{
	static int moment = 0;

	switch(moment)
	{
		default: moment = 0;
		case 0: enqueueLinearMovement(SPEED, 500); break;
		case 1: enqueueLinearMovement(SPEED, -500); break;
		case 2: enqueueTurn(SPEED, 0, 90); break;
		case 3: enqueueTurn(SPEED, 0, -90); break;
	}

	++moment;
	return;

	enqueueLinearMovement(SPEED, 250);
	enqueueTurn(SPEED, 0, -180);
	enqueueLinearMovement(SPEED, 250);
	enqueueTurn(SPEED, 0, 180);
	enqueueLinearMovement(SPEED, 250);
}

void handleTest(void)
{
	//	if(moment == 0 && isMovementComplete())
	//	{
	//		++moment;
	//		initiateLinearMovement(SPEED, 500);
	//	}
	//	else if(moment == 2 && isMovementComplete())
	//	{
	//		++moment;
	//		initiateLinearMovement(SPEED, -100);
	//	}
	//	else if(moment == 4 && isMovementComplete())
	//	{
	//		++moment;
	//		initiateTurn(SPEED, 0, 90);
	//	}
	//	else if(moment == 6 && isMovementComplete())
	//	{
	//		++moment;
	//		initiateTurn(SPEED, 0, -90);
	//	}
	//	else if(moment == 8 && isMovementComplete())
	//	{
	//		++moment;
	//		initiateTurn(SPEED, 100, 90);
	//	}
	//	else if(moment == 10 && isMovementComplete())
	//	{
	//		++moment;
	//		initiateTurn(SPEED, 0, 360);
	//	}
	//	else if(moment == 12 && isMovementComplete())
	//	{
	//		++moment;
	//		initiateTurn(SPEED, 0, 450);
	//	}
	//	else if(moment == 14 && isMovementComplete())
	//	{
	//		++moment;
	//		initiateTurn(SPEED, 0, 4000);
	//	}
}
