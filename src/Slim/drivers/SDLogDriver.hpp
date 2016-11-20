#ifndef __SD_LOG_DRIVER_H
#define __SD_LOG_DRIVER_H

#include "DualEncoderDriver.hpp"
#include "../modules/OrientationSensorsWrapper.hpp"
#include "../utility/Point3D.hpp"

void initLogger();
void logDataPack(OrientationSensors&, DualEncoder&, DualEncoder&);
void dumpLog();
void dropLog();

#endif // __SD_LOG_DRIVER_H
