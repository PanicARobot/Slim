#ifndef __PLANAR_ACCELERATION_MODULE_H
#define __PLANAR_ACCELERATION_MODULE_H

#include "../utility/Point3D.hpp"
#include "../modules/OrientationSensorsWrapper.hpp"

extern Point3D<float> acceleration_vector;

void updatePlanarAcceleration(OrientationSensors&);

#endif // __PLANAR_ACCELERATION_MODULE_H
