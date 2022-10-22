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
#include "material.h"
#include "moving_sphere.h"
#include "bvh.h"
#include "aarect.h"
#include "box.h"
#include "constant_medium.h"

//构建场景
Scene cornell_box();


//初始化最终图像
Film film{ 500,500,3};
//Film film{};
const int samples_per_pixel = 100;
const int max_depth = 50;
float cameraFov = 40.0;
const double minTime = 0.;
const double maxTime = 1.;



RGBColor ray_color(Ray& r,const Hittable& world, int depth, RGBColor bgColor) {
    Hit_record rec{};


    if (depth <= 0)
        return RGBColor(0, 0, 0);

    if (world.hit(r, 0.001, infinity,rec)) {
        
        Ray scattered{};
        //RGBColor attenuation{};
        RGBColor emitted = rec.mat_ptr->emitted(rec.uv.first, rec.uv.second, rec.p);

        double pdf;
        RGBColor albedo;

        if (rec.mat_ptr->scatter(r, rec, albedo, scattered, pdf)) {
            return emitted
                + albedo * rec.mat_ptr->scatter_pdf(r, rec, scattered)
                * ray_color(scattered ,world, depth - 1, bgColor) / pdf;
            //return emitted + attenuation * ray_color(scattered, world, depth - 1, bgColor);
        }
        else {
            return emitted;
        }
    }
    else {
        //Vec3 unit_direction = unit_vector(r.m_dir);
        //auto t = 0.5 * (unit_direction.y() + 1.0);
        //return (1.0 - t) * RGBColor(1.0, 1.0, 1.0) + t * RGBColor(0.5, 0.7, 1.0);
        return bgColor;
    }

    
}

int main()
{   
    Scene scene{};
    RGBColor bgColor{ 0., 0., 0. };
    CameraFrame cFrame{ {278,278,-800},{278,278,0},{0,1,0} };


    scene = cornell_box();

    
    // World
    Scene world{ scene };
    //BVH_NODE world{ { scene } ,minTime ,maxTime };


    //Camera camera{ {{-2,2,1},{0,0,-1},{0,1,0}}, cameraFov, film.getAspectRadio() };
    RealCamera camera{ cFrame, cameraFov, film.getAspectRadio(),0.,1.,0,10.};


    const int imageCH = film.imageCH;
    const int imageCW = film.imageCW;
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
                pixel_color += ray_color(r, world, max_depth, bgColor);
            }

            film.setPixWithGamma(y, x, pixel_color, samples_per_pixel);
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



Scene cornell_box() {
    Scene objects;

    auto red = make_shared<Lambertian>(RGBColor(.65, .05, .05));
    auto white = make_shared<Lambertian>(RGBColor(.73, .73, .73));
    auto green = make_shared<Lambertian>(RGBColor(.12, .45, .15));
    auto light = make_shared<DiffuseLight>(RGBColor(15, 15, 15));

    objects.add(make_shared<YZ_Rect>(0, 555, 0, 555, 555, green));
    objects.add(make_shared<YZ_Rect>(0, 555, 0, 555, 0, red));
    objects.add(make_shared<XZ_Rect>(213, 343, 227, 332, 554, light));
    objects.add(make_shared<XZ_Rect>(0, 555, 0, 555, 0, white));
    objects.add(make_shared<XZ_Rect>(0, 555, 0, 555, 555, white));
    objects.add(make_shared<XY_Rect>(0, 555, 0, 555, 555, white));


    //objects.add(std::make_shared<Box>(Point3(130, 0, 65), Point3(295, 165, 230), white));
    //objects.add(std::make_shared<Box>(Point3(265, 0, 295), Point3(430, 330, 460), white));


    std::shared_ptr<Hittable> box1 = std::make_shared<Box>(Point3(0, 0, 0), Point3(165, 330, 165), white);
    box1 = std::make_shared<Rotate_Y>(box1, 15);
    box1 = std::make_shared<Translate>(box1, Vec3(265, 0, 295));
    
    objects.add(box1);

    std::shared_ptr<Hittable> box2 = std::make_shared<Box>(Point3(0, 0, 0), Point3(165, 165, 165), white);
    box2 = std::make_shared<Rotate_Y>(box2, -18);
    box2 = std::make_shared<Translate>(box2, Vec3(130, 0, 65));
    objects.add(box2);

    return objects;
}

