#ifndef __TIRE_CONTACT_MODULE_H
#define __TIRE_CONTACT_MODULE_H

#include "OrientationSensorsWrapper.hpp"

#include <cstdint>

// a module that determines whether the tires have contact to the surface, based on AHRS data

// read left tire contact state
uint8_t getLeftTireContactState(OrientationSensors&);

// read right tire contact state
uint8_t getRightTireContactState(OrientationSensors&);

#endif // __TIRE_CONTACT_MODULE_H
