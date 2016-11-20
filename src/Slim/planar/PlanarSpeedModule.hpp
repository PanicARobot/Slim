#ifndef __PLANAR_SPEED_MODULE_H
#define __PLANAR_SPEED_MODULE_H

#include "../modules/OrientationSensorsWrapper.hpp"
#include "../utility/Point3D.hpp"

extern Point3D<float> speed_vector;

void initPlanarSpeed();
void updatePlanarSpeed(OrientationSensors&);

#endif // __PLANAR_SPEED_MODULE_H
