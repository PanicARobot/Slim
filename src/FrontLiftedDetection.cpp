
#include "FrontLiftedDetection.h"

// local threshold keeping
static float frontLiftedThreshold_local;

// init the module trough a set threshold
void initFrontLifted(float frontLiftedThreshold)
{
    frontLiftedThreshold_local = frontLiftedThreshold;
}

// evaluate front scoop lifted state
uint8_t getFrontLiftedState()
{
    if(position.getRoll() > frontLiftedThreshold_local)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}