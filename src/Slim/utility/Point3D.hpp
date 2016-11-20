#ifndef __POINT_3D_H
#define __POINT_3D_H

#include <cmath>

template<typename T>
struct Point3D {
	T x, y, z;

	inline void operator+=(const Point3D& other) {
		x += other.x;
		y += other.y;
		z += other.z;
	}

	inline void operator-=(const Point3D& other) {
		x -= other.x;
		y -= other.y;
		z -= other.z;
	}

	inline void operator*=(T mul) {
		x *= mul;
		y *= mul;
		z *= mul;
	}

	inline void operator/=(T div) {
		x /= div;
		y /= div;
		z /= div;
	}

	inline void rotatePR(float pitch, float roll) {
		float cp = cos(pitch), cr = cos(roll);
		float sp = sin(pitch), sr = sin(roll);

		float nx =  cp * x + sp * sr * y + sp * cr * z;
		float ny =           cr * y -           sr * z;
		float nz = -sp * x + cp * sr * y - cp * cr * z;

		x = nx; y = ny; z = nz;
	}

	inline void rotateYPR(float yaw, float pitch, float roll) {
		float cy = cos(yaw), cp = cos(pitch), cr = cos(roll);
		float sy = sin(yaw), sp = sin(pitch), sr = sin(roll);

		float nx = cy * cp * x + (cy * sp * sr - sy * cr) * y + (cy * sp * cr + sy * sr) * z;
		float ny = sy * cp * x + (sy * sp * sr + cy * cr) * y + (sy * sp * cr - cy * sr) * z;
		float nz =    - sp * x +       cp * sr            * y -       cp * cr            * z;

		x = nx; y = ny; z = nz;
	}
};

#endif // __POINT_3D_H
