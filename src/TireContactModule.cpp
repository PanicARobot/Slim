#include "TireContactModule.h"

#include "OrientationSensorsWrapper.h"
#include "Constants.h"

// a module that determines whether the tires have contact to the surface, based on AHRS data

static float thresholdForTireLostContact;

// init the module
void tiresContactInit(float thresholdForLostOfContact)
{
    thresholdForTireLostContact = thresholdForLostOfContact;
}

// read left tire contact state
uint8_t getLeftTireContactState()
{
    // valid, tested
    if(position.getPitch() * 180 / M_PI > thresholdForTireLostContact)
    {
        return 0;
    }
    else if((position.getAccX() * position.getAccX() + position.getAccY() * position.getAccY() + position.getAccZ() * position.getAccZ()) < ZERO_G_DETECTION_THRESHOLD)
    {
        return 0;
    }
    else
    {
        return 1;
    }
}

// read right tire contact state
uint8_t getRightTireContactState()
{
    // valid, tested
    if(position.getPitch() * 180 / M_PI < -thresholdForTireLostContact)
    {
        return 0;
    }
    else if((position.getAccX() * position.getAccX() + position.getAccY() * position.getAccY() + position.getAccZ() * position.getAccZ()) < ZERO_G_DETECTION_THRESHOLD)
    {
        return 0;
    }
    else
    {
        return 1;
    }
}
