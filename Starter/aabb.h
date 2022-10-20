#pragma once
#include "ray.h"
#include "vec3.h"


class AABB {
public:
	AABB() {}
	AABB(const Point3& a, const Point3& b) { minP = a; maxP = b; }

	bool hit(const Ray& r, double t_min, double t_max) const {

		double t_x0 = (minP.x() - r.m_ori.x()) / r.m_dir.x();
		double t_x1 = (maxP.x() - r.m_ori.x()) / r.m_dir.x();
		if (t_x0 > t_x1) std::swap(t_x0, t_x1);

		double t_y0 = (minP.y() - r.m_ori.y()) / r.m_dir.y();
		double t_y1 = (maxP.y() - r.m_ori.y()) / r.m_dir.y();
		if (t_y0 > t_y1) std::swap(t_y0, t_y1);


		double t_z0 = (minP.z() - r.m_ori.z()) / r.m_dir.z();
		double t_z1 = (maxP.z() - r.m_ori.z()) / r.m_dir.z();
		if (t_z0 > t_z1) std::swap(t_z0, t_z1);

		double crossPmax = std::min(t_x1, std::min(t_y1, t_z1));
		double crossPmin = std::max(t_x0, std::max(t_y0, t_z0));

		double r_min{ std::max(t_min ,crossPmin) };
		double r_max{ std::min(t_max ,crossPmax) };

		return r_min < r_max;



		
	}
public:
	Point3 maxP;
	Point3 minP;
};

AABB surrounding_box(AABB box0, AABB box1) {
	Point3 small(std::min(box0.minP.x(), box1.minP.x()),
		std::min(box0.minP.y(), box1.minP.y()),
		std::min(box0.minP.z(), box1.minP.z()));

	Point3 big(std::max(box0.maxP.x(), box1.maxP.x()),
		std::max(box0.maxP.y(), box1.maxP.y()),
		std::max(box0.maxP.z(), box1.maxP.z()));

	return AABB(small, big);
}