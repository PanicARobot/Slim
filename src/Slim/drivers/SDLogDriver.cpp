#include "SDLogDriver.hpp"

#include <SD.h>

#include <cstdint>

#define LOG_FILENAME            "LOG"
#define BEGIN_LIFE_BYTE         'B'
#define BEGIN_DATA_BYTE         'b'

static struct {
	uint32_t timestamp;
	float acc_x, acc_y, acc_z;
	float gyro_x, gyro_y, gyro_z;
	float ahrs_roll, ahrs_pitch, ahrs_yaw;
	float left_speed, right_speed;
	float planar_acc_x, planar_acc_y, planar_acc_z;
	float planar_speed_x, planar_speed_y, planar_speed_z;
} log_data_pack;

static File f;

static char* const data_begin = (char*)(&log_data_pack); // Yes, yes, I know
static const int data_length = sizeof(log_data_pack);

void initLogger()
{
	f = SD.open(LOG_FILENAME, FILE_WRITE);
	f.write(BEGIN_LIFE_BYTE);
	f.flush();
}

void logDataPack(OrientationSensors& position,
		DualEncoder& leftEncoder, DualEncoder& rightEncoder,
		const Point3D<float>& planar_acc, const Point3D<float>& planar_speed)
{
	log_data_pack.timestamp = millis();

	log_data_pack.acc_x = position.getAccX();
	log_data_pack.acc_y = position.getAccY();
	log_data_pack.acc_z = position.getAccZ();

	log_data_pack.gyro_x = position.getGyroX();
	log_data_pack.gyro_y = position.getGyroY();
	log_data_pack.gyro_z = position.getGyroZ();

	log_data_pack.ahrs_roll = position.getRoll();
	log_data_pack.ahrs_pitch = position.getPitch();
	log_data_pack.ahrs_yaw = position.getYaw();

	log_data_pack.left_speed = leftEncoder.getSpeed();
	log_data_pack.right_speed = rightEncoder.getSpeed();

	log_data_pack.planar_acc_x = planar_acc.x;
	log_data_pack.planar_acc_y = planar_acc.y;
	log_data_pack.planar_acc_z = planar_acc.z;

	log_data_pack.planar_speed_x = planar_speed.x;
	log_data_pack.planar_speed_y = planar_speed.y;
	log_data_pack.planar_speed_z = planar_speed.z;

	f.write(BEGIN_DATA_BYTE);
	f.write(data_begin, data_length);

	// TODO: implement own buffering
	// f.flush();
}

void dumpLog()
{
	f.close();

	f = SD.open(LOG_FILENAME, FILE_READ);

	while(f.peek() >= 0)
	{
		char c = f.read();
		if(c == BEGIN_LIFE_BYTE)
		{
			Serial.println("=== UNIT BOOTUP ===");
			Serial.print("timestamp,acc x,acc y,acc z,gyro x,gyro y,gyro z,roll,pitch,yaw,left enc,right enc,planar acc x,planar acc y,planar acc z,planar speed x,planar speed y,planar speed z\n");
		}
		else if(c == BEGIN_DATA_BYTE)
		{
			for(int i = 0; i < data_length; ++i)
				data_begin[i] = f.read();

			Serial.print(log_data_pack.timestamp); Serial.print(",");

			// Accelerometer in g
			Serial.print(log_data_pack.acc_x); Serial.print(",");
			Serial.print(log_data_pack.acc_y); Serial.print(",");
			Serial.print(log_data_pack.acc_z); Serial.print(",");

			// Gyroscope in m / s^2
			Serial.print(log_data_pack.gyro_x); Serial.print(",");
			Serial.print(log_data_pack.gyro_y); Serial.print(",");
			Serial.print(log_data_pack.gyro_z); Serial.print(",");

			// AHRS in radians
			Serial.print(log_data_pack.ahrs_roll); Serial.print(",");
			Serial.print(log_data_pack.ahrs_pitch); Serial.print(",");
			Serial.print(log_data_pack.ahrs_yaw); Serial.print(",");

			// Encoders in mm / s
			Serial.print(log_data_pack.left_speed); Serial.print(",");
			Serial.print(log_data_pack.right_speed); Serial.print(",");

			// Planar acceleration ???
			Serial.print(log_data_pack.planar_acc_x); Serial.print(",");
			Serial.print(log_data_pack.planar_acc_y); Serial.print(",");
			Serial.print(log_data_pack.planar_acc_z); Serial.print(",");

			// Planar speed ???
			Serial.print(log_data_pack.planar_speed_x); Serial.print(",");
			Serial.print(log_data_pack.planar_speed_y); Serial.print(",");
			Serial.print(log_data_pack.planar_speed_z); Serial.print("\n");
		}
		else
		{
			Serial.println("Log file is corrupt");
			break;
		}
	}

	Serial.println("=== DUMP COMPLETE ===");

	f.close();

	SD.remove(LOG_FILENAME);

	initLogger();
}

void dropLog()
{
	f.close();

	Serial.println("=== LOG ERASED ===");

	SD.remove(LOG_FILENAME);

	initLogger();
}
