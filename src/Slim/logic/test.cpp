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
		initiateLinearMovement(200, 100);
	}
	if(moment == 1 && isMovementComplete())
	{
		++moment;
		initiateLinearMovement(200, -100);
	}
}
