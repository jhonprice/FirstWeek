#pragma once
#include "vec3.h"
class Ray {
public:
	Ray(Point3 ori, Vec3 dir) :m_ori(ori), m_dir(dir) {}

	Point3 at(double t) {
		return Point3{ m_ori + t * m_dir };
	}
public:
	Point3 m_ori;
	Vec3 m_dir;
};