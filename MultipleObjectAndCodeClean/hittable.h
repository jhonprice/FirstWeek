#pragma once

#include "../RayCameraAndBackGround/ray.h"

class material;

//<<�������еļ�¼>>
//+=<<�жϹ�������������λ�ù�ϵ��ȷ�����߳���>>
//+=<<������ɢ������>> - 1
struct hit_record {
    point3 p;
    vec3 normal;
    shared_ptr<material> mat_ptr; // 1
    double t;

    bool front_face;

    //<<�жϹ�������������λ�ù�ϵ��ȷ�����߳���>>
    inline void set_face_normal(const ray& r, const vec3& outward_normal) {
        front_face = dot(r.direction(), outward_normal) < 0;
        normal = front_face ? outward_normal : -outward_normal;
    }
};


//<<�жϹ�����һ��������Χ���Ƿ�hit>>
class hittable {
public:
    virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const = 0;
};