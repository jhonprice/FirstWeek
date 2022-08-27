#pragma once
#include "../Starter/vec3.h"
#include "../Starter/color.h"

//<<光线的表示>>
class ray {
//<<初始化部分>>
public:
    ray() {}
    ray(const point3& origin, const vec3& direction)
        : orig(origin), dir(direction)
    {}
//<<光线组成成员的访问>>
    point3 origin() const { return orig; }
    vec3 direction() const { return dir; }
//<<光线上的点的参数化表示>>
    point3 at(double t) const {
        return orig + t * dir;
    }
//<<光线由光源和其方向的向量组成>>
public:
    point3 orig;
    vec3 dir;
};