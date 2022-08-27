#pragma once

#include "hittable.h"
#include "../Starter/vec3.h"


//<<将球体与光线相交的逻辑抽象出来>>
class sphere : public hittable {
//<<球体构造>>
public:
    sphere() {}
    sphere(point3 cen, double r) : center(cen), radius(r) {};

    virtual bool hit(
        const ray& r, double t_min, double t_max, hit_record& rec) const override;
//<<球体参数>>
public:
    point3 center;
    double radius;
};


//<<判断光线在一定参数范围内是否hit>> - 1
//+=<<判断并更新法线朝向>> - 2 
bool sphere::hit(const ray& r, double t_min, double t_max, hit_record& rec) const {
//<<计算相交方程的根>>
#pragma region Calculate
    vec3 oc = r.origin() - center;
    auto a = r.direction().length_squared();
    auto half_b = dot(oc, r.direction());
    auto c = oc.length_squared() - radius * radius;

    auto discriminant = half_b * half_b - a * c;
    if (discriminant < 0) return false;
    auto sqrtd = sqrt(discriminant);
    auto root = (-half_b - sqrtd) / a;
#pragma endregion

//<<在一定参数范围内是否hit,并计算相交纪录>>
//+= 2：<<判断并更新法线朝向>>
#pragma region MyRegion
    //1: Find the nearest root that lies in the acceptable range.
    if (root < t_min || t_max < root) {
        root = (-half_b + sqrtd) / a;
        if (root < t_min || t_max < root)
            return false;
    }
    //1: 相交的参数
    rec.t = root;
    rec.p = r.at(rec.t);

    //2：<<判断并更新法线朝向>>
    //rec.normal = (rec.p - center) / radius;
    vec3 outward_normal = (rec.p - center) / radius;
    rec.set_face_normal(r, outward_normal);
    return true;
#pragma endregion

}