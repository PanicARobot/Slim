#ifndef __PLANAR_SPEED_MODULE_H
#define __PLANAR_SPEED_MODULE_H

#include "../OrientationSensorsWrapper.hpp"

extern Point3D<float> speed_vector;

void initPlanarSpeed(OrientationSensors&);
void updatePlanarSpeed(OrientationSensors&);

#endif // __PLANAR_SPEED_MODULE_H
