#ifndef __PROXIMITY_SENSORS_H
#define __PROXIMITY_SENSORS_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void initProximitySensors(void);
uint8_t readLeftSensor(void);
uint8_t readRightSensor(void);

#ifdef __cplusplus
}
#endif

#endif // __PROXIMITY_SENSORS_H
