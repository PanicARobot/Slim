#ifndef __DUAL_ENCODER_DRIVER_H
#define __DUAL_ENCODER_DRIVER_H

#include <cstdint>
#include <cmath>

#define IMPULSES_PER_ROUND   12
#define WHEEL_REDUCTION   29.86
#define WHEEL_DIAMETER       18


class DualEncoder {
	private:
		static constexpr float WHEEL_PERIMETER = WHEEL_DIAMETER * M_PI;
		static constexpr float IMPULSES_PER_WHEEL = IMPULSES_PER_ROUND * WHEEL_REDUCTION;
		static constexpr float DISTANCE_PER_IMPULSE = MICROS_PER_SECOND * WHEEL_PERIMETER / IMPULSES_PER_WHEEL;

		uint32_t last_micros;
		int8_t impulse_counter;

		uint32_t impulse_deltas[IMPULSES_PER_ROUND];
		uint32_t impulse_deltas_sum;
		uint8_t impulse_deltas_index;

		float speed;

		const int A_PIN;
		const int B_PIN;

		void update(int8_t);

	public:
		DualEncoder(int, int);

		void init(void (*)(), void (*)());
		void update();

		void A_handler();
		void B_handler();

		inline float getSpeed() { return speed; }; // mm / s
};

#endif // __DUAL_ENCODER_DRIVER_H
