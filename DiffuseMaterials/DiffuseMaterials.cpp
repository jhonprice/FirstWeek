// DiffuseMaterials.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//


#include <iostream>
#include "../MultipleObjectAndCodeClean/rtweekend.h"
#include "../MultipleObjectAndCodeClean/hittable_list.h"
#include "../MultipleObjectAndCodeClean/sphere.h"
#include "../Antialiasing/camera.h"

//<<设置全局宽高比>>
const auto aspect_ratio = 16.0 / 9.0;

//<<光线投射，构建背景,可视化法线，计算颜色>>
//+=<<产生随机方向来组成漫反射的方向>>
//+=<<漫反射作为第二次的光线继续投射>>
//+=<<限制递归次数来防止堆栈爆炸>>
//+=<<Fixing Shadow Acne>>
color ray_color(const ray& r, const hittable& world, int depth);
color ray_color2(const ray& r, const hittable& world, int depth);

int main()
{
    //<<图片Image的尺寸设置>>
    //+=<<每个像素的采样次数>>
#pragma region ImageInfo
    const int image_width = 400;
    const int image_height = static_cast<int>(image_width / aspect_ratio);
    //<<每个像素的采样次数>>
    const int samples_per_pixel = 100;
    //<<限制递归次数来防止堆栈爆炸>>
    const int max_depth = 50;
#pragma endregion

    //<<构建场景：初始化可命中对象列表>>
    hittable_list world;
    world.add(make_shared<sphere>(point3(0, 0, -1), 0.5));
    world.add(make_shared<sphere>(point3(0, -100.5, -1), 100));


    //<<Camera相机视口参数设置>>
#pragma region CameraInfo
    camera cam;
#pragma endregion


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
                pixel_color += ray_color2(r, world, max_depth);
            }
            write_color(std::cout, pixel_color, samples_per_pixel);
#pragma endregion     

        }
    }
    std::cerr << "\nDone.\n";
#pragma endregion

}

//<<光线投射，构建背景,可视化法线，计算颜色>>
//+=<<产生随机方向来组成漫反射的方向>>
//+=<<漫反射作为第二次的光线继续投射>>
//+=<<限制递归次数来防止堆栈爆炸>>
//+=<<Fixing Shadow Acne>>
color ray_color(const ray& r, const hittable& world, int depth) {
    hit_record rec;


    //<<限制递归次数来防止堆栈爆炸>>
    if (depth <= 0)
        return color(0, 0, 0);

    //<<Fixing Shadow Acne>>
    if (world.hit(r, 0.001, infinity, rec)) {
        //<<产生随机方向来组成漫反射的方向>>
        //+=<<漫反射作为第二次的光线继续投射>>
#pragma region DiffuseTarget
        point3 target = rec.p + rec.normal + random_in_unit_sphere();
        return 0.5 * ray_color(ray(rec.p, target - rec.p), world, depth-1);
#pragma endregion
    }


    vec3 unit_direction = unit_vector(r.direction());
    auto t = 0.5 * (unit_direction.y() + 1.0);
    return (1.0 - t) * color(1.0, 1.0, 1.0) + t * color(0.5, 0.7, 1.0);
}



color ray_color2(const ray& r, const hittable& world, int depth) {
    hit_record rec;

    if (depth <= 0)
        return color(0, 0, 0);

    //<<Fixing Shadow Acne>>
    if (world.hit(r, 0.001, infinity, rec)) {

#pragma region DiffuseTarget
        point3 target = rec.p + rec.normal + unit_vector(random_in_unit_sphere());
        return 0.5 * ray_color(ray(rec.p, target - rec.p), world, depth - 1);
#pragma endregion
    }


    vec3 unit_direction = unit_vector(r.direction());
    auto t = 0.5 * (unit_direction.y() + 1.0);
    return (1.0 - t) * color(1.0, 1.0, 1.0) + t * color(0.5, 0.7, 1.0);
}

