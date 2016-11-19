#ifndef __POINT_3D_H
#define __POINT_3D_H

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
		x *= div;
		y *= div;
		z *= div;
	}
};

#endif // __POINT_3D_H
