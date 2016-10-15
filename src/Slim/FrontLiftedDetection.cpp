#include "FrontLiftedDetection.h"

#include <cmath>

#define FRONT_LIFTED_THRESHOLD       2.00

uint8_t getFrontLiftedState(OrientationSensors& position)
{
    if(position.getRoll() * 180 / M_PI > FRONT_LIFTED_THRESHOLD)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}
