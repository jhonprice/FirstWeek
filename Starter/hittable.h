#pragma once
#include "ray.h"
#include "aabb.h"
class Material;

//记录交点信息和材质
struct Hit_record {
    Point3 p{};
    Vec3 normal{};
    double t{};
    bool front_face{};
    std::pair<double, double> uv{};

    std::shared_ptr<Material> mat_ptr;

    inline void set_face_normal(const Ray& r, const Vec3& outward_normal) {
        front_face = dot(r.m_dir, outward_normal) < 0;
        normal = front_face ? outward_normal : -outward_normal;
    }
};

class Hittable {
public:
    virtual bool bounding_box(double time0, double time1, AABB& output_box) const = 0;
    virtual bool hit(const Ray& r, double t_min, double t_max, Hit_record& rec) const = 0;

    virtual double pdf_value(const Point3& o, const Vec3& v) const {
        return 0.0;
    }

    virtual Vec3 random(const Vec3& o) const {
        return Vec3(1, 0, 0);
    }
};
using Shape = Hittable;
class Translate : public Hittable {
public:
    Translate(std::shared_ptr<Hittable> p, const Vec3& displacement)
        : ptr(p), offset(displacement) {}

    virtual bool hit(const Ray& r, double t_min, double t_max,Hit_record& rec) const override;

    virtual bool bounding_box(double time0, double time1, AABB& output_box) const override;

public:
    std::shared_ptr<Hittable> ptr;
    Vec3 offset;
};

bool Translate::hit(const Ray& r, double t_min, double t_max, Hit_record& rec) const {
    Ray moved_r(r.m_ori - offset, r.m_dir, r.m_time);
    if (!ptr->hit(moved_r, t_min, t_max, rec))
        return false;

    rec.p += offset;
    rec.set_face_normal(moved_r, rec.normal);

    return true;
}

bool Translate::bounding_box(double time0, double time1, AABB& output_box) const {
    if (!ptr->bounding_box(time0, time1, output_box))
        return false;

    output_box = AABB(
        output_box.minP + offset,
        output_box.maxP + offset);

    return true;
};

class Rotate_Y : public Hittable {
public:
    Rotate_Y(std::shared_ptr<Hittable> p, double angle);

    virtual bool hit(const Ray& r, double t_min, double t_max, Hit_record& rec) const override;

    virtual bool bounding_box(double time0, double time1, AABB& output_box) const override {
        output_box = bbox;
        return hasbox;
    }
public:
    std::shared_ptr<Hittable> ptr;
    double sin_theta;
    double cos_theta;
    bool hasbox;
    AABB bbox;
};

Rotate_Y::Rotate_Y(std::shared_ptr<Hittable> p, double angle) : ptr(p) {
    auto radians = degrees_to_radians(angle);
    sin_theta = sin(radians);
    cos_theta = cos(radians);
    // 获取被变换的hittable物体的外围盒，并记录它是否有外围盒，绝大部分物体都有外围盒
    hasbox = ptr->bounding_box(0, 1, bbox);

    // 新的外围盒的最小坐标值与最大坐标值，即左下角和右上角
    constexpr double  infinity = std::numeric_limits<double>::infinity();
    Point3 min(infinity, infinity, infinity);
    Point3 max(-infinity, -infinity, -infinity);

    // 对于被变换物体的包围盒的8个顶点
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 2; j++) {
            for (int k = 0; k < 2; k++) {
                auto x = i * bbox.maxP.x() + (1 - i) * bbox.minP.x();
                auto y = j * bbox.maxP.y() + (1 - j) * bbox.minP.y();
                auto z = k * bbox.maxP.z() + (1 - k) * bbox.minP.z();

                // 算出当前顶点的新的x, y坐标值
                auto newx = cos_theta * x + sin_theta * z;
                auto newz = -sin_theta * x + cos_theta * z;

                // 根据新的x,y坐标值来跟新min 和 max坐标值
                Vec3 tester(newx, y, newz);

                for (int c = 0; c < 3; c++) {
                    min[c] = std::min(min[c], tester[c]);
                    max[c] = std::max(max[c], tester[c]);
                }
            }
        }
    }

    bbox = AABB(min, max);
}

bool Rotate_Y::hit(const Ray& r, double t_min, double t_max, Hit_record& rec) const {
    // 射线源点
    auto origin = r.m_ori;
    // 射线方向
    auto direction = r.m_dir;

    // 射线原点先经过 -θ的旋转
    origin[0] = cos_theta * r.m_ori[0] - sin_theta * r.m_ori[2];
    origin[2] = sin_theta * r.m_ori[0] + cos_theta * r.m_ori[2];

    // 射线方向也是先经过 -θ的旋转
    direction[0] = cos_theta * r.m_dir[0] - sin_theta * r.m_dir[2];
    direction[2] = sin_theta * r.m_dir[0] + cos_theta * r.m_dir[2];
    // 旋转变换过的射线rotated_r
    Ray rotated_r(origin, direction, r.m_time);

    // 用旋转变换过的射线与原始物体进行碰撞检测，如果没有碰撞则返回false
    if (!ptr->hit(rotated_r, t_min, t_max, rec))
        return false;

    // 算出的碰撞点p
    auto p = rec.p;
    // 算出的碰撞法线normal
    auto normal = rec.normal;

    // 碰撞点p需要正向旋转θ
    p[0] = cos_theta * rec.p[0] + sin_theta * rec.p[2];
    p[2] = -sin_theta * rec.p[0] + cos_theta * rec.p[2];

    // 碰撞法线normal也需要正向旋转θ
    normal[0] = cos_theta * rec.normal[0] + sin_theta * rec.normal[2];
    normal[2] = -sin_theta * rec.normal[0] + cos_theta * rec.normal[2];

    // 记录新的碰撞点和碰撞法线
    rec.p = p;
    rec.set_face_normal(rotated_r, normal);

    return true;
}


class Flip_face : public Hittable {
public:
    Flip_face(std::shared_ptr<Hittable> p) : ptr(p) {}

    virtual bool hit(
        const Ray& r, double t_min, double t_max, Hit_record& rec) const override {

        if (!ptr->hit(r, t_min, t_max, rec))
            return false;

        rec.front_face = !rec.front_face;
        return true;
    }

    virtual bool bounding_box(double time0, double time1, AABB& output_box) const override {
        return ptr->bounding_box(time0, time1, output_box);
    }

public:
    std::shared_ptr<Hittable> ptr;
};