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

//初始化最终图像
Film film;

RGBColor ray_color(const Ray& r) {
    Vec3 unit_direction = unit_vector(r.m_dir);
    auto t = 0.5 * (unit_direction.y() + 1.0);
    return (1.0 - t) * RGBColor(1.0, 1.0, 1.0) + t * RGBColor(0.5, 0.7, 1.0);
}

int main()
{
    // Render：输出一张图片，列变化控制r分量，行变化控制g分量，b分量保持0.25
    const int imageCH = film.imageCH;
    const int imageCW = film.imageCW;


    //Camera
    auto viewport_height = 2.0;
    auto viewport_width = film.getAspectRadio() * viewport_height;
    auto focal_length = 1.0;

    //Camera坐标系下光的参数
    auto origin = Point3(0, 0, 0);
    auto horizontal = Vec3(viewport_width, 0, 0);
    auto vertical = Vec3(0, viewport_height, 0);
    auto lower_left_corner = origin - horizontal / 2 - vertical / 2 - Vec3(0, 0, focal_length);

    //渲染循环
    auto start = system_clock::now();
    for (int y{ 0 }; y < imageCH; y++) {
        std::cout << "\rScanlines remaining: " << y << ' ' << std::flush;
        for (int x{ 0 }; x < imageCW; x++) {
            auto u = double(x) / (imageCW - 1);
            auto v = double(y) / (imageCH - 1);

            //逻辑：世界坐标->相机坐标->ray_color根据y的变化进行插值 
            Vec3 rayOrigin = origin;
            Vec3 rayHit = lower_left_corner + u * horizontal + v * vertical - origin;
            Ray r{ rayOrigin, rayHit };
            film.setPix(y,x,ray_color(r));

            RGBColor pixColor;

  
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
