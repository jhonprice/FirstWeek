#pragma once
#include "hittable.h"
#include "material.h"
#include "texture.h"

class Constant_medium : public Hittable {
public:
    Constant_medium(std::shared_ptr<Hittable> b, double d, std::shared_ptr<Texture> a)
        : boundary(b),
        neg_inv_density(-1 / d),
        phase_function(std::make_shared<Isotropic>(a))
    {}

    Constant_medium(std::shared_ptr<Hittable> b, double d, RGBColor c)
        : boundary(b),
        neg_inv_density(-1 / d),
        phase_function(std::make_shared<Isotropic>(c))
    {}

    virtual bool hit(const Ray& r, double t_min, double t_max, Hit_record& rec) const override;

    virtual bool bounding_box(double time0, double time1, AABB& output_box) const override {
        return boundary->bounding_box(time0, time1, output_box);
    }

public:
    static constexpr double  infinity = std::numeric_limits<double>::infinity();
    std::shared_ptr<Hittable> boundary;
    std::shared_ptr<Material> phase_function;
    double neg_inv_density;
};


bool Constant_medium::hit(const Ray& r, double t_min, double t_max, Hit_record& rec) const {
    // Print occasional samples when debugging. To enable, set enableDebug true.
    const bool enableDebug = false;
    const bool debugging = enableDebug && random_double() < 0.00001;

    Hit_record rec1, rec2;


    // 首先判断如果射线两端都延长至无限长，是否能与这个体积体的包围盒相交
    if (!boundary->hit(r, -infinity, infinity, rec1))
        // 如果不相交则返回false
        return false;



    // 看射线在第一次相交后， 另一端延长至无限长，是否能体积体第二次相交，如果不能第二次相交则返回false
    if (!boundary->hit(r, rec1.t + 0.0001, infinity, rec2))
        return false;

    if (debugging) std::cerr << "\nt_min=" << rec1.t << ", t_max=" << rec2.t << '\n';

    // 把两次相交的时间限制在 t_min和t_max之间
    if (rec1.t < t_min) rec1.t = t_min;
    if (rec2.t > t_max) rec2.t = t_max;

    // 如果第一次相交的时间比第二次还要大，则返回false
    if (rec1.t >= rec2.t)
        return false;


    // 把第一次相交的时间限制在0以上，即射线的正向端
    if (rec1.t < 0)
        rec1.t = 0;

    // 射线总长度
    const auto ray_length = r.m_dir.length();
    // 射线在体积体内的长度
    const auto distance_inside_boundary = (rec2.t - rec1.t) * ray_length;
    // 随机一个可能性 并除以 密度 ，随机算出一个碰撞距离差
    const auto hit_distance = neg_inv_density * log(random_double());

    // 如果当前射线在体积体内的长度 比 当前随机算出的碰撞距离差 还要短，则返回false
    if (hit_distance > distance_inside_boundary)
        return false;

    // 碰撞时间 = 第一次碰撞点的时间t + 距离差/射线总长度
    rec.t = rec1.t + hit_distance / ray_length;
    rec.p = r.at(rec.t);

    if (debugging) {
        std::cerr << "hit_distance = " << hit_distance << '\n'
            << "rec.t = " << rec.t << '\n'
            << "rec.p = " << rec.p << '\n';
    }

    rec.normal = Vec3(1, 0, 0);  // 碰撞法线随意
    rec.front_face = true;     // 是碰撞到前面了
    rec.mat_ptr = phase_function;

    return true;
}