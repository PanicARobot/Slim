#ifndef __DUAL_ENCODER_DRIVER_H
#define __DUAL_ENCODER_DRIVER_H

#include <cstdint>

class Encoder {
	private:
		uint32_t last_micros;
		int8_t impulse_counter;

		float speed;

		void update(uint8_t);

	public:
		Encoder();

		void update();
		inline float getSpeed() { return speed; }; // mm / s

		friend void leftAHandler();
		friend void leftBHandler();
		friend void rightAHandler();
		friend void rightBHandler();
};

extern Encoder leftEncoder, rightEncoder;

void initDualEncoders();

#endif // __DUAL_ENCODER_DRIVER_H
