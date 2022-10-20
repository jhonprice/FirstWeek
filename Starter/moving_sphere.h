#pragma once
#include "hittable.h"
#include "vec3.h"
#include "ray.h"


class MovingSphere : public Shape {
public:
    MovingSphere() {}
    MovingSphere(Point3 cen, Point3 cen2, double _time0, double _time1, double r, std::shared_ptr<Material> m)
        : m_ori(cen), m_ori_moved(cen2), time0(_time0), time1(_time1),m_r(r), mat_ptr(m) {};
    MovingSphere(Point3 cen, Point3 cen2, double _time0, double _time1, double r) : m_ori(cen), m_ori_moved(cen2), time0(_time0), time1(_time1),m_r(r) {};

    virtual bool hit(
        Ray& ray, Hit_record& rec) const override;
    Point3 center(double time) const;

    virtual bool bounding_box(
        double _time0, double _time1, AABB& output_box) const override;
public:
    Point3 m_ori, m_ori_moved;
    double time0, time1;
    double m_r;
    std::shared_ptr<Material> mat_ptr;
};

Point3 MovingSphere::center(double time) const {
    return m_ori + ((time - time0) / (time1 - time0)) * (m_ori_moved - m_ori);
}

bool MovingSphere::hit(Ray& ray, Hit_record& rec) const {
    Vec3 oc = ray.m_ori - center(ray.m_time);

    auto a = dot(ray.m_dir, ray.m_dir);
    auto b = 2.0 * dot(oc, ray.m_dir);
    auto c = dot(oc, oc) - m_r * m_r;

    auto discriminant = b * b - 4 * a * c;
    if (discriminant < 0) {
        return false;
    }
    else {
        rec.t = (-b - sqrt(discriminant)) / (2.0 * a);

        if (rec.t < ray.tMin || ray.tMax < rec.t)
            return false;
    }

    rec.p = ray.at(rec.t);
    auto outward_normal = (rec.p - center(ray.m_time)) / m_r;
    rec.set_face_normal(ray, outward_normal);  //ÉèÖÃrec.normal
    rec.mat_ptr = mat_ptr; //ÉèÖÃ²ÄÖÊ

    return true;
}

bool MovingSphere::bounding_box(double _time0, double _time1, AABB& output_box) const {
    AABB box0(
        center(_time0) - Vec3(m_r, m_r, m_r),
        center(_time0) + Vec3(m_r, m_r, m_r));
    AABB box1(
        center(_time1) - Vec3(m_r, m_r, m_r),
        center(_time1) + Vec3(m_r, m_r, m_r));
    output_box = surrounding_box(box0, box1);
    return true;
}