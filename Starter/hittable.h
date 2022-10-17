#pragma once
#include "ray.h"

struct Hit_record {
    Point3 p;
    Vec3 normal;
    double t;
    bool front_face;

    inline void set_face_normal(const Ray& r, const Vec3& outward_normal) {
        front_face = dot(r.m_dir, outward_normal) < 0;
        normal = front_face ? outward_normal : -outward_normal;
    }
};

class Hittable {
public:
    virtual bool hit(Ray& r, Hit_record& rec) const = 0;
};
using Shape = Hittable;
