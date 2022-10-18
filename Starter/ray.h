#pragma once
#include "vec3.h"
class Ray {
public:
	Ray(){};
	Ray(const Point3& ori,const Vec3& dir,double time =0.0) :m_ori(ori), m_dir(dir),m_time(time) {}

	Point3 at(double t) const{
		return Point3{ m_ori + t * m_dir };
	}
public:
	Point3 m_ori;
	Vec3 m_dir;
	double tMin{0.001};
	double tMax{std::numeric_limits<double>::infinity()};

	double m_time;
};