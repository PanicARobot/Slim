#ifndef __SD_LOG_DRIVER_H
#define __SD_LOG_DRIVER_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
	uint32_t timestamp;
	float acc_x, acc_y, acc_z;
	float gyro_x, gyro_y, gyro_z;
	float ahrs_x, ahrs_y, ahrs_z;
	float left_speed, right_speed;
} __LOG_DATA;

extern __LOG_DATA log_data_pack;

void initLogger(void);
void logDataPack(void);
void dumpLog(void);
void dropLog(void);

#ifdef __cplusplus
}
#endif

#endif // __SD_LOG_DRIVER_H
