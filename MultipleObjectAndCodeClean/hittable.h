#pragma once

#include "../RayCameraAndBackGround/ray.h"

class material;

//<<光线命中的记录>>
//+=<<判断光线与表面的内外位置关系来确定法线朝向>>
//+=<<材质与散射属性>> - 1
struct hit_record {
    point3 p;
    vec3 normal;
    shared_ptr<material> mat_ptr; // 1
    double t;

    bool front_face;

    //<<判断光线与表面的内外位置关系来确定法线朝向>>
    inline void set_face_normal(const ray& r, const vec3& outward_normal) {
        front_face = dot(r.direction(), outward_normal) < 0;
        normal = front_face ? outward_normal : -outward_normal;
    }
};


//<<判断光线在一定参数范围内是否hit>>
class hittable {
public:
    virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const = 0;
};