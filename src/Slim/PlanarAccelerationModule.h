#ifndef __PLANAR_ACCELERATION_MODULE_H
#define __PLANAR_ACCELERATION_MODULE_H

#include <cmath>

class PlanarAcceleration {
	private:
		float acc_x;
		float acc_y;

	public:
		void update();

		inline float getX() { return acc_x; }
		inline float getY() { return acc_y; }
};

extern PlanarAcceleration planarAcceleration;

#endif // __PLANAR_ACCELERATION_MODULE_H
