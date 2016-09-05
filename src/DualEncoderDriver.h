#ifndef __DUAL_ENCODER_DRIVER_H
#define __DUAL_ENCODER_DRIVER_H

#include <cmath>
#include <cstdint>

class Encoder {
	private:
		int direction;
		uint32_t last_micros;
		int impulse_counter;
		float speed;

		void update(int);

	public:
		Encoder();

		void updateSpeed();
		inline int getDirection() { return direction; } // 1 is forward, 0 is backward
		inline float getSpeed() { return speed; }; // mm / s

		friend void leftHandler();
		friend void rightHandler();
};

extern Encoder leftEncoder, rightEncoder;

void initDualEncoders();

#endif // __DUAL_ENCODER_DRIVER_H
