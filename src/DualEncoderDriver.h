#ifndef __DUAL_ENCODER_DRIVER_H
#define __DUAL_ENCODER_DRIVER_H

#include <cstdint>

class Encoder {
	private:
		uint32_t last_micros;
		int8_t impulse_counter;

		float speed;
		float acceleration;

		void update(uint8_t);

	public:
		Encoder();

		void update();
		inline float getSpeed() { return speed; }; // mm / s
		inline float getAcc() { return acceleration; }; // mm / s^2

		friend void leftAHandler();
		friend void leftBHandler();
		friend void rightAHandler();
		friend void rightBHandler();
};

extern Encoder leftEncoder, rightEncoder;

void initDualEncoders();

#endif // __DUAL_ENCODER_DRIVER_H
