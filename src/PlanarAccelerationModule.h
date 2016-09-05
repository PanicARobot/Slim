#ifndef __PLANAR_ACCELERATION_MODULE_H
#define __PLANAR_ACCELERATION_MODULE_H

#include <cmath>

class PlanarAcceleration {
	private:
		float acc_offset_x;
		float acc_offset_y;

		float acc_x;
		float acc_y;

	public:
		void calibrate();

		void update();

		inline float getAcc()
		{
			return sqrt(acc_x * acc_x + acc_y * acc_y);
		}
		inline float getAngle()
		{
			return atan2(-acc_x, acc_y) * 180 / M_PI;
		}
};

extern PlanarAcceleration planarAcceleration;

#endif // __PLANAR_ACCELERATION_MODULE_H
