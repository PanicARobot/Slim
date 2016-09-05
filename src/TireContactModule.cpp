#include "TireContactModule.h"

#include "OrientationSensorsWrapper.h"
#include "constants.h"

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
    // TODO validate
    if(position.getPitch() > thresholdForTireLostContact)
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
    // TODO validate
    if(position.getPitch() < -thresholdForTireLostContact)
    {
        return 0;
    }
    else
    {
        return 1;
    }
}
