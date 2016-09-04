#ifndef __DUAL_ENCODER_DRIVER_H
#define __DUAL_ENCODER_DRIVER_H

#include <cmath>

#define WHEEL_PERIMETER (18 * M_PI)
#define IMPULSES_PER_ROUND 358.32
#define MICROS_PER_SECOND 1000000


class Encoder {
	private:
		static constexpr float DISTANCE = MICROS_PER_SECOND * WHEEL_PERIMETER / IMPULSES_PER_ROUND;

		int direction;
		int lastReadingMicros;

	public:
		Encoder();

		inline void update(int b) { direction = b; }
		inline int getDirection() { return direction; } // 1 is forward, 0 is backward
		int getSpeed();
};

extern Encoder leftEncoder, rightEncoder;

void initDualEncoders();

#endif // __DUAL_ENCODER_DRIVER_H
