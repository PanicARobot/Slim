#include "test.h"

#include "../drivers/MotorDriver.h"
#include "../drivers/ProximitySensors.h"
#include "../control/Movement.h"

static int moment = 0;

void initTest()
{
	moment = 0;
}

void handleTest(void)
{
	if(moment == 0 && isMovementComplete())
	{
		++moment;
		initiateLinearMovement(400, 100);
	}
	else if(moment == 1 && isMovementComplete())
	{
		++moment;
		initiateLinearMovement(400, -100);
	}
	else if(moment == 2 && isMovementComplete())
	{
		++moment;
		initiateTurn(400, 0, 90);
	}
	else if(moment == 3 && isMovementComplete())
	{
		++moment;
		initiateTurn(400, 0, -90);
	}
	else if(moment == 4 && isMovementComplete())
	{
		++moment;
		initiateTurn(400, 100, 90);
	}
}
