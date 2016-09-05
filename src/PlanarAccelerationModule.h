#ifndef __PLANAR_ACCELERATION_MODULE_H
#define __PLANAR_ACCELERATION_MODULE_H

class PlanarAcceleration {
	private:
		float acc_offset_x;
		float acc_offset_y;

	public:
		void calibrate();
};

extern PlanarAcceleration plannarAcceleration;

#endif // __PLANAR_ACCELERATION_MODULE_H
