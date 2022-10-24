#pragma once
#include "hittable.h"
#include "vec3.h"
#include "ray.h"
#include "onb.h"
#include "pdf.h"

class Sphere : public Shape {
public:
    Sphere() {}
    Sphere(Point3 cen, double r, std::shared_ptr<Material> m)
        : m_ori(cen), m_r(r), mat_ptr(m) {};
    Sphere(Point3 cen, double r) : m_ori(cen), m_r(r) {};

    virtual bool hit(const Ray& ray, double t_min, double t_max, Hit_record& rec) const override;
    virtual bool bounding_box(double time0, double time1, AABB& output_box) const override;
    virtual double pdf_value(const Point3& o, const Vec3& v) const override;
    virtual Vec3 random(const Vec3& o) const override;

    static void get_sphere_uv(const Point3& p, double& u, double& v) {
        // p: a given point on the sphere of radius one, centered at the origin.
        // u: returned value [0,1] of angle around the Y axis from X=-1.
        // v: returned value [0,1] of angle from Y=-1 to Y=+1.
        //     <1 0 0> yields <0.50 0.50>       <-1  0  0> yields <0.00 0.50>
        //     <0 1 0> yields <0.50 1.00>       < 0 -1  0> yields <0.50 0.00>
        //     <0 0 1> yields <0.25 0.50>       < 0  0 -1> yields <0.75 0.50>

        auto theta = acos(-p.y());
        auto phi = atan2(-p.z(), p.x()) + pi;

        u = phi / (2 * pi);
        v = theta / pi;
    }

public:
    Point3 m_ori;
    double m_r;
    std::shared_ptr<Material> mat_ptr;
};

bool Sphere::hit(const Ray& ray, double t_min, double t_max, Hit_record& rec) const {
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

        if (rec.t < t_min || t_max < rec.t)
            return false;
    }
    
    rec.p = ray.at(rec.t);

    Vec3 outward_normal = (rec.p - m_ori) / m_r;
    rec.set_face_normal(ray, outward_normal);  //ÉèÖÃrec.normal

    get_sphere_uv(outward_normal, rec.uv.first, rec.uv.second);
    rec.mat_ptr = mat_ptr; //ÉèÖÃ²ÄÖÊ

    return true;
}

bool Sphere::bounding_box(double time0, double time1, AABB& output_box) const {
    output_box = AABB(
        m_ori - Vec3(m_r, m_r, m_r),
        m_ori + Vec3(m_r, m_r, m_r));
    return true;
}


double Sphere::pdf_value(const Point3& o, const Vec3& v) const {
    Hit_record rec;
    if (!this->hit(Ray(o, v), 0.001, infinity, rec))
        return 0;

    auto cos_theta_max = sqrt(1 - m_r * m_r / (m_ori - o).length_squared());
    auto solid_angle = 2 * pi * (1 - cos_theta_max);

    return  1 / solid_angle;
}

inline Vec3 Sphere::random(const Vec3& o) const
{
    Vec3 direction = m_ori - o;
    auto distance_squared = direction.length_squared();
    ONB uvw;
    uvw.build_from_w(direction);
    return uvw.local(random_to_sphere(m_r, distance_squared));
}