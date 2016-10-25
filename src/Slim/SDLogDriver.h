#ifndef __SD_LOG_DRIVER_H
#define __SD_LOG_DRIVER_H

#include "OrientationSensorsWrapper.h"
#include "DualEncoderDriver.h"

void initLogger();
void logDataPack(OrientationSensors&, DualEncoder&, DualEncoder&);
void dumpLog();
void dropLog();

#endif // __SD_LOG_DRIVER_H
