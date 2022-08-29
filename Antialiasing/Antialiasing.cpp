// Antialiasing.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include "../MultipleObjectAndCodeClean/rtweekend.h"
#include "../MultipleObjectAndCodeClean/hittable_list.h"
#include "../MultipleObjectAndCodeClean/sphere.h"
#include "camera.h"

//<<设置全局宽高比>>
const auto aspect_ratio = 16.0 / 9.0;

//<<光线投射，构建背景,可视化法线，计算颜色>>
color ray_color(const ray& r, const hittable& world);

int main()
{
//<<图片Image的尺寸设置>>
//+=<<每个像素的采样次数>>
#pragma region ImageInfo
    const int image_width = 400;
    const int image_height = static_cast<int>(image_width / aspect_ratio);
    //<<每个像素的采样次数>>
    const int samples_per_pixel = 100;
#pragma endregion

//<<构建场景：初始化可命中对象列表>>
#pragma region buildWorld
    hittable_list world;
    world.add(make_shared<sphere>(point3(0, 0, -1), 0.5));
    world.add(make_shared<sphere>(point3(0, -100.5, -1), 100));
#pragma endregion


    //<<Camera相机视口参数设置>>
    camera cam;


//<<Render渲染循环，输出到PPM文件>>
//+=<<利用周围像素颜色反走样>>
#pragma region RenderPart
    std::cout << "P3\n" << image_width << " " << image_height << "\n255\n";

    for (int j = image_height - 1; j >= 0; --j) {
        std::cerr << "\rScanlines remaining: " << j << ' ' << std::flush;
        for (int i = 0; i < image_width; ++i) {
            //auto u = double(i) / (image_width - 1);
            //auto v = double(j) / (image_height - 1);
            //ray r(origin, lower_left_corner + u * horizontal + v * vertical - origin);
            //color pixel_color = ray_color(r,world);
            //write_color(std::cout, pixel_color);
            
//<<利用周围像素颜色反走样>>
#pragma region RenderWithAntiAliasing
            color pixel_color(0, 0, 0);
            for (int s = 0; s < samples_per_pixel; ++s) {
                auto u = (i + random_double()) / (image_width - 1);
                auto v = (j + random_double()) / (image_height - 1);
                ray r = cam.get_ray(u, v);
                pixel_color += ray_color(r, world);
            }
            write_color(std::cout, pixel_color, samples_per_pixel);
#pragma endregion     
            
        }
    }
    std::cerr << "\nDone.\n";
#pragma endregion

}

//<<光线投射，构建背景,可视化法线，计算颜色>>
color ray_color(const ray& r, const hittable& world) {
    hit_record rec;
    if (world.hit(r, 0, infinity, rec)) {
        return 0.5 * (rec.normal + color(1, 1, 1));
    }
    vec3 unit_direction = unit_vector(r.direction());
    auto t = 0.5 * (unit_direction.y() + 1.0);
    return (1.0 - t) * color(1.0, 1.0, 1.0) + t * color(0.5, 0.7, 1.0);
}

