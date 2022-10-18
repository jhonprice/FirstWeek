#pragma once
#include "vec3.h"
#include "ray.h"
struct CameraFrame {
    Point3 lookfrom{0,0,0};
    Point3 lookat{0,0,-1};
    Vec3   vup{0,1,0};
};

class Camera {
public:
    Camera(CameraFrame frame,double vfov,// 垂直的fov - 度数表示
        double aspect_ratio) {

        auto theta = degrees_to_radians(vfov);
        auto h = tan(theta / 2);

        auto viewport_height = 2.0* h;
        auto viewport_width = aspect_ratio * viewport_height;

        auto w = unit_vector(frame.lookfrom - frame.lookat);
        auto u = unit_vector(cross(frame.vup, w));
        auto v = cross(w, u);
        


        origin = frame.lookfrom;
        horizontal = viewport_width * u;
        vertical = viewport_height * v;
        lower_left_corner = origin - horizontal / 2 - vertical / 2 - w;
    }

    Ray get_ray(double s, double t) const {
        return Ray(origin, lower_left_corner + s * horizontal + t * vertical - origin);
    }

protected:
    Point3 origin;
    Point3 lower_left_corner;
    Vec3 horizontal;
    Vec3 vertical;
};