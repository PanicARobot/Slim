
// include APP to driver connector
#include "DriverAppConnection.h"

// Slip of a wheel will be defined as a rapid change in the speed of the wheel in the direction that is currently going
// the threshold for this driver is defined in % from current speed

#define SPEED_INCREASE_MAX_PERCENT      10

// local defines
#define NO_SLIP_OCCURRED                0
#define SLIP_OCCURRED                   1

// local data

// buffers for previous values, needed so comparison is possible
static int32_t lastRightWheelRPM, lastLeftWheelRPM;

// init function for the driver
void initSlipDetection(void)
{
    // init signals
    leftWheelSlipOccurred  = NO_SLIP_OCCURRED;
    rightWheelSlipOccurred = NO_SLIP_OCCURRED;

    // init local buffers
    lastRightWheelRPM = 0;
    lastLeftWheelRPM = 0;
}

// periodic function for sleep detection (10Hz call is needed for proper work)
void evaluateSlipDetection(void)
{
    // local buffer for derivatives for both motors
    int32_t leftMotorSpeedDerivative, rightMotorSpeedDerivative;

    // calculate derivatives for both motors
    rightMotorSpeedDerivative = lastRightWheelRPM - rightWheelRPM;
    leftMotorSpeedDerivative = lastLeftWheelRPM - leftWheelRPM;

    // evaluate right motor
    if  ((1 == rightMotorDir && (SPEED_INCREASE_MAX_PERCENT * rightWheelRPM / 100) < rightMotorSpeedDerivative) ||
        (0 == rightMotorDir && (-SPEED_INCREASE_MAX_PERCENT * rightWheelRPM / 100) > rightMotorSpeedDerivative))
    {
        rightWheelSlipOccurred = 1;
    }
    else
    {
        rightWheelSlipOccurred = 0;
    }

    // evaluate left motor
    if  ((1 == leftMotorDir && (SPEED_INCREASE_MAX_PERCENT * leftWheelRPM / 100) < leftMotorSpeedDerivative) ||
        (0 == leftMotorDir && (-SPEED_INCREASE_MAX_PERCENT * leftWheelRPM / 100) > leftMotorSpeedDerivative))
    {
        leftWheelSlipOccurred = 1;
    }
    else
    {
        leftWheelSlipOccurred = 0;
    }

    // load last values in buffers
    lastRightWheelRPM = rightWheelRPM;
    lastLeftWheelRPM = leftWheelRPM;
}
