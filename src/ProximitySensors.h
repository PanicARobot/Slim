#ifndef __PROXIMITY_SENSORS_DRIVER_H
#define __PROXIMITY_SENSORS_DRIVER_H

#include <cstdint>

void initProximitySensors();
void readProximitySensors(uint8_t&, uint8_t&);

#endif // __PROXIMITY_SENSORS_DRIVER_H
