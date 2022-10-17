// Starter.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#define STB_IMAGE_WRITE_IMPLEMENTATION
#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <array>
#include <chrono>
using namespace std::chrono;

#include "image.h"
#include "ray.h"
#include "sphere.h"
#include "math_helper.h"
#include "hittable_list.h"
#include "camera.h"



//初始化最终图像
Film film{};
const int samples_per_pixel = 20;

RGBColor ray_color(Ray& r,const Scene& world) {
    Hit_record rec{};
    //if (Sphere{ {0,0,-1},0.5 }.hit(r, rec)) {
    //    return 0.5 * (rec.normal + RGBColor(1, 1, 1));
    //}

    if (world.hit(r, rec)) {
        return 0.5 * (rec.normal + RGBColor(1, 1, 1));
    }

    Vec3 unit_direction = unit_vector(r.m_dir);
    auto t = 0.5 * (unit_direction.y() + 1.0);
    return (1.0 - t) * RGBColor(1.0, 1.0, 1.0) + t * RGBColor(0.5, 0.7, 1.0);
}

int main()
{
    // Render：输出一张图片，列变化控制r分量，行变化控制g分量，b分量保持0.25
    const int imageCH = film.imageCH;
    const int imageCW = film.imageCW;


    // World
    Scene world;
    world.add(make_shared<Sphere>(Point3(0, 0, -1), 0.5));
    world.add(make_shared<Sphere>(Point3(0, -100.5, -1), 100));



    Camera camera{ film.getAspectRadio() };

    //渲染循环
    auto start = system_clock::now();
    for (int y{ 0 }; y < imageCH; ++y) {
        std::cout << "\rScanlines remaining: " << y << ' ' << std::flush;
        for (int x{ 0 }; x < imageCW; ++x) {
            RGBColor pixel_color(0, 0, 0);

            for (int s = 0; s < samples_per_pixel; ++s) {
                auto u = double(x + random_double()) / (imageCW - 1);
                auto v = double(y + random_double()) / (imageCH - 1);
                Ray r{ camera.get_ray(u,v) };
                pixel_color += ray_color(r, world);
            }

            film.setPix(y, x, pixel_color, samples_per_pixel);
        }
    }
    auto end = system_clock::now();
    auto duration = duration_cast<microseconds>(end - start);

    //写入文件
    // 默认从上到下，从左到右,现在翻转（00为左下角
    film.writeTgaFile("./test.tga");

    //报告完成
    std::cout << "\nDone: "<< double(duration.count()) * microseconds::period::num / microseconds::period::den<<" s" << std::endl;
    return 0;
}
