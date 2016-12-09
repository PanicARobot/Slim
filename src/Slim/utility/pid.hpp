#ifndef __PID_H
#define __PID_H

class PidController {
	private:
		const float kP, kI, kD;
		float last_error, integral;
		float integral_flank;

	public:
		PidController(float, float, float, float);

		void zero();
		float sample(float, float, float);
};

#endif
