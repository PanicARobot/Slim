#ifndef __SD_LOG_DRIVER_H
#define __SD_LOG_DRIVER_H

#include "OrientationSensorsWrapper.hpp"
#include "DualEncoderDriver.hpp"

void initLogger();
void logDataPack(OrientationSensors&, DualEncoder&, DualEncoder&);
void dumpLog();
void dropLog();

#endif // __SD_LOG_DRIVER_H
