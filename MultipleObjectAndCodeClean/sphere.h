#pragma once

#include "hittable.h"
#include "../Starter/vec3.h"


//<<������������ཻ���߼��������>>
class sphere : public hittable {
//<<���幹��>>
public:
    sphere() {}
    sphere(point3 cen, double r) : center(cen), radius(r) {};

    virtual bool hit(
        const ray& r, double t_min, double t_max, hit_record& rec) const override;
//<<�������>>
public:
    point3 center;
    double radius;
};


//<<�жϹ�����һ��������Χ���Ƿ�hit>> - 1
//+=<<�жϲ����·��߳���>> - 2 
bool sphere::hit(const ray& r, double t_min, double t_max, hit_record& rec) const {
//<<�����ཻ���̵ĸ�>>
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

//<<��һ��������Χ���Ƿ�hit,�������ཻ��¼>>
//+= 2��<<�жϲ����·��߳���>>
#pragma region MyRegion
    //1: Find the nearest root that lies in the acceptable range.
    if (root < t_min || t_max < root) {
        root = (-half_b + sqrtd) / a;
        if (root < t_min || t_max < root)
            return false;
    }
    //1: �ཻ�Ĳ���
    rec.t = root;
    rec.p = r.at(rec.t);

    //2��<<�жϲ����·��߳���>>
    //rec.normal = (rec.p - center) / radius;
    vec3 outward_normal = (rec.p - center) / radius;
    rec.set_face_normal(r, outward_normal);
    return true;
#pragma endregion

}