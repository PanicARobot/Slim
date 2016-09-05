#ifndef __DUAL_ENCODER_DRIVER_H
#define __DUAL_ENCODER_DRIVER_H

#include <cmath>
#include <cstdint>

class Encoder {
	private:
		uint32_t last_micros;
		uint32_t impulse_counter;

		int8_t direction;
		float speed;
		float acceleration;

		void update(int8_t);

	public:
		Encoder();

		void update();
		inline int getDirection() { return direction; } // 1 is forward, 0 is backward
		inline float getSpeed() { return speed; }; // mm / s
		inline float getAcc() { return acceleration; }; // mm / s^2

		friend void leftHandler();
		friend void rightHandler();
};

extern Encoder leftEncoder, rightEncoder;

void initDualEncoders();

#endif // __DUAL_ENCODER_DRIVER_H
