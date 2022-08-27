// RayCameraAndBackGround.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include "../Starter/vec3.h"
#include "../Starter/color.h"
#include "ray.h"

//<<设置全局的宽高比>>
const auto aspect_ratio = 16.0 / 9.0;

//<<光线投射，构建背景>> - 1
color ray_color(const ray& r);

//<<光线投射与球体相交>> - 2
bool hit_sphere(const point3& center, double radius, const ray& r);


int main()
{
//<<根据宽高比设置图片的高度与宽度>>
#pragma region ImageInfo
    const int image_width = 400;
    const int image_height = static_cast<int>(image_width / aspect_ratio);
#pragma endregion

//<<Camera相机视口参数设置>>
#pragma region CameraInfo
    auto viewport_height = 2.0;
    auto viewport_width = aspect_ratio * viewport_height;
    auto focal_length = 1.0; // z轴距离


    auto origin = point3(0, 0, 0);
    auto horizontal = vec3(viewport_width, 0, 0);
    auto vertical = vec3(0, viewport_height, 0);
    auto lower_left_corner = origin - horizontal / 2 - vertical / 2 - vec3(0, 0, focal_length);
#pragma endregion


//<<Render渲染循环，输出到PPM文件>>
#pragma region RenderPart
    std::cout << "P3\n" << image_width << " " << image_height << "\n255\n";

    for (int j = image_height - 1; j >= 0; --j) {
        std::cerr << "\rScanlines remaining: " << j << ' ' << std::flush;
        for (int i = 0; i < image_width; ++i) {
            auto u = double(i) / (image_width - 1);
            auto v = double(j) / (image_height - 1);
            ray r(origin, lower_left_corner + u * horizontal + v * vertical - origin);
            color pixel_color = ray_color(r);
            write_color(std::cout, pixel_color);
        }
    }
    std::cerr << "\nDone.\n";
#pragma endregion

}


//1:<<光线投射，构建背景,击中球体>>
/*
*   理想结果：y轴从底下的白色到上面的蓝色渐变，与球体相交部分是深蓝色
*/
color ray_color(const ray& r)
{
//<<与球体相交>>
#pragma region SpherePart
    auto sphere_center = point3(0, 0, -1);
    auto sphere_r = 0.5;
    if (hit_sphere(sphere_center, sphere_r, r))
        return color(0, 0, 1);
#pragma endregion


//<<构建背景>>
#pragma region BackgroudColor
    vec3 unit_direction = unit_vector(r.direction());
    auto t = 0.5 * (unit_direction.y() + 1.0);
    return (1.0 - t) * color(1.0, 1.0, 1.0) + t * color(0.5, 0.7, 1.0);
#pragma endregion

}



//2:<<光线投射与球体相交:求解方程判别式>>
bool hit_sphere(const point3& center, double radius, const ray& r)
{
    vec3 oc = r.origin() - center;
    auto a = dot(r.direction(), r.direction());
    auto b = 2.0 * dot(oc, r.direction());
    auto c = dot(oc, oc) - radius * radius;
    auto discriminant = b * b - 4 * a * c;
    return (discriminant > 0);
}
