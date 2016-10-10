#ifndef __TIRE_CONTACT_MODULE_H
#define __TIRE_CONTACT_MODULE_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// a module that determines whether the tires have contact to the surface, based on AHRS data

// read left tire contact state
uint8_t getLeftTireContactState(void);

// read right tire contact state
uint8_t getRightTireContactState(void);

#ifdef __cplusplus
}
#endif

#endif // __TIRE_CONTACT_MODULE_H
