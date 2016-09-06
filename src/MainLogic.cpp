#include "MainLogic.h"
#include "MotorDriver.h"
#include "DualEncoderDriver.h"
#include "FrontLiftedDetection.h"

#include "Constants.h"

#define STANDARD_SPEED				       120
#define SPEED_CHANGE_STEP			       1
#define DISTANCE_BETWEEN_MOTORS		       85.00
#define HALF_DISTANCE_BETWEEN_MOTORS       (DISTANCE_BETWEEN_MOTORS / 2.00)


enum {
	SEARCH_INITIAL,
    SEARCH_TURNING,
    SEARCH_FORWARD,

    EXIT_STATE_ON_SIDE,
    EXIT_STATE_ON_IMPACT,
    EXIT_STATE_OUT_OF_RING,
    EXIT_STATE_FRONT_LIFTED,

	STOP_MOVING
} STATE = SEARCH_INITIAL;

enum {
	MOVEMENT_SYSTEM_STATUS_OFF,
	MOVEMENT_SYSTEM_STATUS_LINEAR_MOVEMENT,
	MOVEMENT_SYSTEM_STATUS_ROUND_MOVEMENT
} movementSystemStatus = MOVEMENT_SYSTEM_STATUS_OFF;

template<typename T> inline void swap(T& a, T& b) { T c = a; a = b; b = c; }

static float distanceExpectedByLeftTire, distanceExpectedByRightTire;

static int16_t leftMotorSpeed, rightMotorSpeed, leftMotorDir, rightMotorDir;
static int8_t directionOfLinearMovement;
static float circleMotorSpeedDifference;


void HandleControlledMovement()
{
	switch(movementSystemStatus)
	{
		case MOVEMENT_SYSTEM_STATUS_OFF:
			// stop the motors, they should not be moving
			setMotors(0, 0);
			break;

		case MOVEMENT_SYSTEM_STATUS_LINEAR_MOVEMENT:
		{
			float leftTyreSpeed = (float)leftEncoder.getSpeed();
			float rightTyreSpeed = (float)rightEncoder.getSpeed();

			// check if the speeds are the same if not correct the speed of the slower motors
			if(leftTyreSpeed > rightTyreSpeed)
			{
				leftMotorSpeed -= SPEED_CHANGE_STEP;
			}
			else if(leftTyreSpeed < rightTyreSpeed)
			{
				leftMotorSpeed += SPEED_CHANGE_STEP;
			}

			// update moved distance
			distanceExpectedByLeftTire -= (float)(leftTyreSpeed / SAMPLE_FREQUENCY);
			distanceExpectedByRightTire -= (float)(rightTyreSpeed / SAMPLE_FREQUENCY);

			if(	(distanceExpectedByLeftTire < 0 && directionOfLinearMovement > 0) ||
				(distanceExpectedByLeftTire > 0 && directionOfLinearMovement < 0) )
			{
				leftMotorSpeed = 0;
			}


			if(	(distanceExpectedByRightTire < 0 && directionOfLinearMovement > 0) ||
				(distanceExpectedByRightTire > 0 && directionOfLinearMovement < 0) )
			{
				rightMotorSpeed = 0;
			}

			if(leftMotorSpeed == 0 && rightMotorSpeed == 0)
			{
				movementSystemStatus = MOVEMENT_SYSTEM_STATUS_OFF;
			}

			setMotors(leftMotorSpeed, rightMotorSpeed);

			break;
		}

		case MOVEMENT_SYSTEM_STATUS_ROUND_MOVEMENT:
		{
			float leftTyreSpeed = leftEncoder.getSpeed();
			float rightTyreSpeed = rightEncoder.getSpeed();

			// check if the speeds are the same if not correct the speed of the slower motors
			if(leftTyreSpeed / rightTyreSpeed > circleMotorSpeedDifference)
			{
				leftMotorSpeed -= SPEED_CHANGE_STEP;
				rightMotorSpeed += SPEED_CHANGE_STEP;
			}
			else if(leftTyreSpeed / rightTyreSpeed < circleMotorSpeedDifference)
			{
				leftMotorSpeed += SPEED_CHANGE_STEP;
				rightMotorSpeed -= SPEED_CHANGE_STEP;
			}

			// update moved distance
			distanceExpectedByLeftTire -= leftTyreSpeed / SAMPLE_FREQUENCY;
			distanceExpectedByRightTire -= rightTyreSpeed / SAMPLE_FREQUENCY;

			if(	(distanceExpectedByLeftTire < 0 && leftMotorDir > 0) ||
				(distanceExpectedByLeftTire > 0 && leftMotorDir < 0) )
			{
				leftMotorSpeed = 0;
			}

			if(	(distanceExpectedByRightTire < 0 && rightMotorDir > 0) ||
				(distanceExpectedByRightTire > 0 && rightMotorDir < 0) )
			{
				rightMotorSpeed = 0;
			}

			if(leftMotorSpeed == 0 && rightMotorSpeed == 0)
			{
				movementSystemStatus = MOVEMENT_SYSTEM_STATUS_OFF;
			}

			setMotors(leftMotorSpeed, rightMotorSpeed);

			break;
		}
	}
}

inline bool IsMovementComplete()
{
	return movementSystemStatus == MOVEMENT_SYSTEM_STATUS_OFF;
}

void LinearMovement(int32_t distanceInMMWithDirection) // TODO: Fix backwards
{
	// evaluate direction and pass to drivers the start state
	if(0 < distanceInMMWithDirection)
	{
		directionOfLinearMovement = 1;
		setMotors(STANDARD_SPEED, STANDARD_SPEED);
	}
	else
	{
		directionOfLinearMovement = -1;
		setMotors(-STANDARD_SPEED, -STANDARD_SPEED);
	}

	// set setpoints for end of movement, used in handler
	distanceExpectedByRightTire = (float)distanceInMMWithDirection;
	distanceExpectedByLeftTire = (float)distanceInMMWithDirection;

	// set motor speeds, used in handler
	rightMotorSpeed = STANDARD_SPEED;
	leftMotorSpeed = STANDARD_SPEED;

	// set state on state machine
	movementSystemStatus = MOVEMENT_SYSTEM_STATUS_LINEAR_MOVEMENT;
}

void Turn(int turnRadius, int turnDegrees) // TODO: Fix when radius != 0
{
	if(turnDegrees == 0) return;

	distanceExpectedByLeftTire  = 2.00 * ((float)turnRadius + HALF_DISTANCE_BETWEEN_MOTORS) * M_PI * (float)turnDegrees / 360;
	distanceExpectedByRightTire = 2.00 * ((float)turnRadius - HALF_DISTANCE_BETWEEN_MOTORS) * M_PI * (float)turnDegrees / 360;
	if(turnDegrees < 0)
	{
		swap(distanceExpectedByLeftTire, distanceExpectedByRightTire);
		distanceExpectedByLeftTire *= -1;
		distanceExpectedByRightTire *= -1;
	}

	circleMotorSpeedDifference = distanceExpectedByLeftTire / distanceExpectedByRightTire;

	leftMotorDir = (turnDegrees > 0 || distanceExpectedByLeftTire > 0) ? 1 : -1;
	rightMotorDir = (turnDegrees < 0 || distanceExpectedByRightTire > 0) ? 1 : -1;

	leftMotorSpeed = STANDARD_SPEED * leftMotorDir;
	rightMotorSpeed = STANDARD_SPEED * rightMotorDir;

	if(turnDegrees > 0) leftMotorSpeed *= STANDARD_SPEED;
	else rightMotorSpeed *= STANDARD_SPEED;

	setMotors(leftMotorSpeed, rightMotorSpeed);

	// set state on state machine
	movementSystemStatus = MOVEMENT_SYSTEM_STATUS_ROUND_MOVEMENT;
}

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

// After Collision Actions
void Push()
{
//    if(STATE == EXIT_STATE_ON_IMPACT)
//    {
//        setMotors(MAX_SPEED,MAX_SPEED);
//
//        //Check Traction TO DO
//        if(!Traction)
//        {
//            Stop();
//            //Wait 50 mil
//            setMotors(MAX_SPEED - SPEED_CONST,MAX_SPEED - SPEED_CONST);
//            //Check C used
//            //if no Traction = 0
//            //if yes Traction = 1
//        }
//    }
//
//
//    //EXITS
//    //Enemy lost
//    if(digitalRead(LEFT_SENSOR_PIN) ^ 1 == 1 || digitalRead(RIGHT_SENSOR_PIN) ^ 1 == 1)
//    STATE = EXIT_STATE_ON_SIDE;
//
//    //lifted front scoop
//    if(getFrontLiftedState)
//    STATE = EXIT_STATE_FRONT_LIFTED;
//    //OUT OF RING
//    if(OutOfRing())
//    STATE = EXIT_STATE_OUT_OF_RING;
//    //Pushed Back TO DO
}
void WaitToPass()
{
    //WTFFFFFF
}
void TurnTowards()
{
//    if(digitalRead(RIGHT_SENSOR_PIN) ^ 1 && IsMovementComplete())
//    Turn(0,90); //turn right
//
//    if(digitalRead(LEFT_SENSOR_PIN) ^ 1 && IsMovementComplete())
//    Turn(0,-90); // turn left
}
void TurnAround()
{
}

void MainLogic()
{
	MovePattern();
	HandleControlledMovement();
}
