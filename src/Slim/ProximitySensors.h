#ifndef __PROXIMITY_SENSORS_DRIVER_H
#define __PROXIMITY_SENSORS_DRIVER_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void initProximitySensors(void);
void readProximitySensors(uint8_t&, uint8_t&);

#ifdef __cplusplus
}
#endif

#endif // __PROXIMITY_SENSORS_DRIVER_H
