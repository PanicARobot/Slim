#ifndef __PID_H
#define __PID_H

class PidController {
	private:
		const float kP, kI, kD;

		const float MIN_SCALE;
		const float MAX_SCALE;

		float last_error, integral;

	public:
		PidController(float, float, float, float, float);

		float sample(float, float, float);
};

#endif
