#pragma once

#include "../MultipleObjectAndCodeClean/rtweekend.h"

//<<����ӿ����õĳ���>>
//+=<<������Ұ�Ĳ���>> - 1
//+=<<lookAt�Ĳ���>> - 2
class camera {
public:
#pragma region CameraInfo
//<<�������ӿ�����>>
	camera() 
    {
		//<<����ȫ�ֿ�߱�>>
		const auto aspect_ratio = 16.0 / 9.0;
        //<<Camera����ӿڲ�������>>
        auto viewport_height = 2.0;
        auto viewport_width = aspect_ratio * viewport_height;
        auto focal_length = 1.0;

        origin = point3(0, 0, 0);
        horizontal = vec3(viewport_width, 0, 0);
        vertical = vec3(0, viewport_height, 0);
        lower_left_corner = origin - horizontal / 2 - vertical / 2 - vec3(0, 0, focal_length);

	}
//<<������Ұ>> - 1
//�ӿڲ��ٹ̶�������vfov�ӽǺ�aspect_ratio�ı仯
    camera(
        double vfov, // vertical field-of-view in degrees
        double aspect_ratio
    ) {
        auto theta = degrees_to_radians(vfov);
        auto h = tan(theta / 2);
        auto viewport_height = 2.0 * h;
        auto viewport_width = aspect_ratio * viewport_height;

        auto focal_length = 1.0;

        origin = point3(0, 0, 0);
        horizontal = vec3(viewport_width, 0.0, 0.0);
        vertical = vec3(0.0, viewport_height, 0.0);
        lower_left_corner = origin - horizontal / 2 - vertical / 2 - vec3(0, 0, focal_length);
    }

//<<lookAt�Ĳ���>> - 2
    camera(
        point3 lookfrom,
        point3 lookat,
        vec3   vup,
        double vfov, // vertical field-of-view in degrees
        double aspect_ratio
    ) {
        auto theta = degrees_to_radians(vfov);
        auto h = tan(theta / 2);
        auto viewport_height = 2.0 * h;
        auto viewport_width = aspect_ratio * viewport_height;

        auto w = unit_vector(lookfrom - lookat);
        auto u = unit_vector(cross(vup, w));
        auto v = cross(w, u);

        origin = lookfrom;
        horizontal = viewport_width * u;
        vertical = viewport_height * v;
        lower_left_corner = origin - horizontal / 2 - vertical / 2 - w;
    }
#pragma endregion

    //<<��ȡ����>>
    //ray get_ray(double u, double v);
    ray get_ray(double u, double v) const {
        return ray(origin, lower_left_corner + u * horizontal + v * vertical - origin);
    }
private:
    point3 origin;
    point3 lower_left_corner;
    vec3 horizontal;
    vec3 vertical;
};