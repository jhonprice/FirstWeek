#pragma once
#include "../Starter/vec3.h"
#include "../Starter/color.h"

//<<���ߵı�ʾ>>
class ray {
//<<��ʼ������>>
public:
    ray() {}
    ray(const point3& origin, const vec3& direction)
        : orig(origin), dir(direction)
    {}
//<<������ɳ�Ա�ķ���>>
    point3 origin() const { return orig; }
    vec3 direction() const { return dir; }
//<<�����ϵĵ�Ĳ�������ʾ>>
    point3 at(double t) const {
        return orig + t * dir;
    }
//<<�����ɹ�Դ���䷽����������>>
public:
    point3 orig;
    vec3 dir;
};