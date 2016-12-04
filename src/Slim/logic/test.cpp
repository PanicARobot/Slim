#include "test.h"

#include "../drivers/MotorDriver.h"
#include "../drivers/ProximitySensors.h"
#include "../control/Movement.h"

#define SPEED 3000

static int moment = -1;

void initTest()
{
	if(moment > 10) moment = 0;
	else ++moment;
}

void handleTest(void)
{
	if(moment == 0 && isMovementComplete())
	{
		++moment;
		initiateLinearMovement(SPEED, 100);
	}
	else if(moment == 2 && isMovementComplete())
	{
		++moment;
		initiateLinearMovement(SPEED, -100);
	}
	else if(moment == 4 && isMovementComplete())
	{
		++moment;
		initiateTurn(SPEED, 0, 90);
	}
	else if(moment == 6 && isMovementComplete())
	{
		++moment;
		initiateTurn(SPEED, 0, -90);
	}
	else if(moment == 8 && isMovementComplete())
	{
		++moment;
		initiateTurn(SPEED, 100, 90);
	}
	else if(moment == 10 && isMovementComplete())
	{
		++moment;
		initiateTurn(SPEED, 0, 360);
	}
}
