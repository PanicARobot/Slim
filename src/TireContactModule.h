#ifndef __TIRE_CONTACT_MODULE_H
#define __TIRE_CONTACT_MODULE_H

#include <cstdint>

// a module that determines whether the tires have contact to the surface, based on AHRS data

// init the module
void tiresContactInit(float);

// read left tire contact state
uint8_t getLeftTireContactState();

// read right tire contact state
uint8_t getRightTireContactState();

#endif // __TIRE_CONTACT_MODULE_H
