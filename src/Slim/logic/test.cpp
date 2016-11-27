#include "test.h"

#include "../drivers/MotorDriver.h"
#include "../drivers/ProximitySensors.h"
#include "../control/Movement.h"

void handleTest(void)
{
	static int moment = 0;

	if(moment == 0 && isMovementComplete())
	{
		++moment;
		initiateTurn(100, 100, 180);
	}
}
