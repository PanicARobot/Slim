#ifndef __DUAL_ENCODER_DRIVER_H
#define __DUAL_ENCODER_DRIVER_H

#include <cmath>
#include <cstdint>

class Encoder {
	private:
		int direction;
		uint32_t lastMicros;
		int impulseCounter;

	public:
		Encoder();

		void update(int);
		inline int getDirection() { return direction; } // 1 is forward, 0 is backward
		float getSpeed();
};

extern Encoder leftEncoder, rightEncoder;

void initDualEncoders();

#endif // __DUAL_ENCODER_DRIVER_H
