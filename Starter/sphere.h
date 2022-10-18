#pragma once
#include "hittable.h"
#include "vec3.h"
#include "ray.h"

class Sphere : public Shape {
public:
    Sphere() {}
    Sphere(Point3 cen, double r, std::shared_ptr<Material> m)
        : m_ori(cen), m_r(r), mat_ptr(m) {};
    Sphere(Point3 cen, double r) : m_ori(cen), m_r(r) {};

    virtual bool hit(
        Ray& ray, Hit_record& rec) const override;

public:
    Point3 m_ori;
    double m_r;
    std::shared_ptr<Material> mat_ptr;
};

bool Sphere::hit(Ray& ray, Hit_record& rec) const {
    Vec3 oc = ray.m_ori - m_ori;

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
    rec.set_face_normal(ray, (rec.p - m_ori) / m_r);  //ÉèÖÃrec.normal
    rec.mat_ptr = mat_ptr; //ÉèÖÃ²ÄÖÊ

    return true;
}