#pragma once
#include "ray.h"

class Material;

//��¼������Ϣ�Ͳ���
struct Hit_record {
    Point3 p;
    Vec3 normal;
    double t;
    bool front_face;

    std::shared_ptr<Material> mat_ptr;

    inline void set_face_normal(const Ray& r, const Vec3& outward_normal) {
        front_face = dot(r.m_dir, outward_normal) < 0;
        normal = front_face ? outward_normal : -outward_normal;
    }
};

class Hittable {
public:
    virtual bool hit(const Ray& r, double t_min, double t_max, Hit_record& rec) const = 0;
};
using Shape = Hittable;
