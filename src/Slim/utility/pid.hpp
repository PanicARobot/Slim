#ifndef __PID_H
#define __PID_H

class PidController {
	private:
		const float kP, kI, kD;
		float last_error, integral;

	public:
		PidController(float, float, float);

		void zero();
		float sample(float, float, float);
};

#endif
