#pragma once
#include "vec3.h"
#include "ray.h"
struct CameraFrame {
    Point3 lookfrom{0,0,0};
    Point3 lookat{0,0,-1};
    Vec3   vup{0,1,0};

    double dist_to_focus() {
        return (lookfrom - lookat).length();
    }
};

class Camera {
public:
    Camera(){}
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

class RealCamera :public Camera {
protected:
    Vec3 u, v, w;
    double lens_radius{};

public:
    //生成一个单位圆平面内的随机向量
    Vec3 random_in_unit_disk() const {
        while (true) {
            auto p = Vec3(random_double(-1, 1), random_double(-1, 1), 0);
            if (p.length_squared() >= 1) continue;
            return p;
        }
    }
    RealCamera(CameraFrame frame, double vfov,// 垂直的fov - 度数表示
        double aspect_ratio, 
        double lenLength) {
        auto focusTodistLength = frame.dist_to_focus();

        auto theta = degrees_to_radians(vfov);
        auto h = tan(theta / 2);

        auto viewport_height = 2.0 * h;
        auto viewport_width = aspect_ratio * viewport_height;

        w = unit_vector(frame.lookfrom - frame.lookat);
        u = unit_vector(cross(frame.vup, w));
        v = cross(w, u);



        origin = frame.lookfrom;
        horizontal = focusTodistLength * viewport_width * u;
        vertical = focusTodistLength * viewport_height * v;
        lower_left_corner = origin - horizontal / 2 - vertical / 2 - focusTodistLength * w;

        lens_radius = lenLength / 2;

    }

    Ray get_ray(double s, double t) const{
        Vec3 rd = lens_radius * random_in_unit_disk();
        Vec3 offset = u * rd.x() + v * rd.y();

        Point3 rayOri = origin + offset;
        return Ray(origin+ offset, lower_left_corner + s * horizontal + t * vertical - rayOri);
    }
};