#ifndef __SD_LOG_DRIVER_H
#define __SD_LOG_DRIVER_H

#include <cstdint>

struct __LOG_DATA {
	uint32_t timestamp;
	float acc_x, acc_y, acc_z;
	float gyro_x, gyro_y, gyro_z;
	float ahrs_x, ahrs_y, ahrs_z;
	float left_speed, right_speed;
};

extern __LOG_DATA log_data_pack;

void initLogger();
void logDataPack();
void dumpLog();
void dropLog();

#endif // __SD_LOG_DRIVER_H
