#pragma once
#include "hittable.h"

class XY_Rect : public Hittable {
public:
    XY_Rect() {}

    XY_Rect(double _x0, double _x1, double _y0, double _y1, double _k,
        std::shared_ptr<Material> mat)
        : x0(_x0), x1(_x1), y0(_y0), y1(_y1), k(_k), mp(mat) {};

    virtual bool hit(Ray& r, Hit_record& rec) const override;

    virtual bool bounding_box(double time0, double time1, AABB& output_box) const override {
        // The bounding box must have non-zero width in each dimension, so pad the Z
        // dimension a small amount.
        output_box = AABB(Point3(x0, y0, k - 0.0001), Point3(x1, y1, k + 0.0001));
        return true;
    }

public:
    std::shared_ptr<Material> mp;
    double x0, x1, y0, y1, k;
};

bool XY_Rect::hit(Ray& r, Hit_record& rec) const {
    auto t = (k - r.m_ori.z()) / r.m_dir.z();
    if (t < r.tMin || t > r.tMax)
        return false;
    auto x = r.m_ori.x() + t * r.m_dir.x();
    auto y = r.m_ori.y() + t * r.m_dir.y();
    if (x < x0 || x > x1 || y < y0 || y > y1)
        return false;


    rec.uv = {(x - x0) / (x1 - x0), (y - y0) / (y1 - y0)};
    rec.t = t;
    rec.set_face_normal(r, Vec3(0, 0, 1));
    rec.mat_ptr = mp;
    rec.p = r.at(t);


    return true;
}


class YZ_Rect : public Hittable {
public:
    YZ_Rect() {}

    YZ_Rect(double _y0, double _y1, double _z0, double _z1, double _k,
        std::shared_ptr<Material> mat)
        : y0(_y0), y1(_y1), z0(_z0), z1(_z1), k(_k), mp(mat) {};

    virtual bool hit(Ray& r, Hit_record& rec) const override;

    virtual bool bounding_box(double time0, double time1, AABB& output_box) const override {
        // The bounding box must have non-zero width in each dimension, so pad the Z
        // dimension a small amount.
        output_box = AABB(Point3(k - 0.0001,y0, z0), Point3(k + 0.0001,y1, z1));
        return true;
    }

public:
    std::shared_ptr<Material> mp;
    double y0, y1, z0, z1, k;
};

bool YZ_Rect::hit(Ray& r, Hit_record& rec) const {
    auto t = (k - r.m_ori.x()) / r.m_dir.x();
    if (t < r.tMin || t > r.tMax)
        return false;
    auto y = r.m_ori.y() + t * r.m_dir.y();
    auto z = r.m_ori.z() + t * r.m_dir.z();
    if (y < y0 || y > y1 || z < z0 || z > z1)
        return false;


    rec.uv = { (y - y0) / (y1 - y0), (z - z0) / (z1 - z0) };
    rec.t = t;
    rec.set_face_normal(r, Vec3(1, 0, 0));
    rec.mat_ptr = mp;
    rec.p = r.at(t);


    return true;
}


class XZ_Rect : public Hittable {
public:
    XZ_Rect() {}

    XZ_Rect(double _x0, double _x1, double _z0, double _z1, double _k,
        std::shared_ptr<Material> mat)
        : x0(_x0), x1(_x1), z0(_z0), z1(_z1), k(_k), mp(mat) {};

    virtual bool hit(Ray& r, Hit_record& rec) const override;

    virtual bool bounding_box(double time0, double time1, AABB& output_box) const override {
        // The bounding box must have non-zero width in each dimension, so pad the Z
        // dimension a small amount.
        output_box = AABB(Point3(x0, z0, k - 0.0001), Point3(x1, z1, k + 0.0001));
        return true;
    }

public:
    std::shared_ptr<Material> mp;
    double x0, x1, z0, z1, k;
};

bool XZ_Rect::hit(Ray& r, Hit_record& rec) const {
    auto t = (k - r.m_ori.y()) / r.m_dir.y();
    if (t < r.tMin || t > r.tMax)
        return false;
    auto x = r.m_ori.x() + t * r.m_dir.x();
    auto z = r.m_ori.z() + t * r.m_dir.z();
    if (x < x0 || x > x1 || z < z0 || z > z1)
        return false;


    rec.uv = { (x - x0) / (x1 - x0), (z - z0) / (z1 - z0) };
    rec.t = t;
    rec.set_face_normal(r, Vec3(0, 1, 0));
    rec.mat_ptr = mp;
    rec.p = r.at(t);


    return true;
}