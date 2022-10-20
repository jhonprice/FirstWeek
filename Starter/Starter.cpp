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

//构建场景
Scene random_scene();
Scene random_scene_test();
Scene two_spheres_scene();
Scene two_perlin_spheres();
Scene earth();


//初始化最终图像
//Film film{400,225,3};
Film film{};
const int samples_per_pixel = 100;
const int max_depth = 50;
const float cameraFov = 20.0;
const double minTime = 0.;
const double maxTime = 1.;

const int imageCH = film.imageCH;
const int imageCW = film.imageCW;

RGBColor ray_color(Ray& r,const Hittable& world, int depth) {
    Hit_record rec{};


    if (depth <= 0)
        return RGBColor(0, 0, 0);

    if (world.hit(r, rec)) {
        
        Ray scattered{};
        RGBColor attenuation{};
        if (rec.mat_ptr->scatter(r, rec, attenuation, scattered))
            return attenuation * ray_color(scattered, world, depth - 1);
        return RGBColor(0, 0, 0);
    }

    Vec3 unit_direction = unit_vector(r.m_dir);
    auto t = 0.5 * (unit_direction.y() + 1.0);
    return (1.0 - t) * RGBColor(1.0, 1.0, 1.0) + t * RGBColor(0.5, 0.7, 1.0);
}

int main()
{   
    Scene scene{};

    switch (4) {
        case 1:
            scene = random_scene();
            break;
        case 2:
            scene = two_spheres_scene();
            break;
        case 3:
            scene = two_perlin_spheres();
            break;
        case 4:
            scene = earth();
            break;
        default:
            scene = random_scene_test();
            break;
    }

    
    // World
    //Scene world{ random_scene() };
    BVH_NODE world{ { scene } ,minTime ,maxTime };


    //Camera camera{ {{-2,2,1},{0,0,-1},{0,1,0}}, cameraFov, film.getAspectRadio() };
    RealCamera camera{{{13,2,3},{0,0,0},{0,1,0}}, cameraFov, film.getAspectRadio(),0.,1.};

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
                pixel_color += ray_color(r, world, max_depth);
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



// 创建一个随机场景
Scene random_scene() {
    Scene world;

    // 地表材质：散射光材质，灰色纯色纹理
    auto ground_material1 = make_shared<Lambertian>(make_shared<SolidColor>(RGBColor(0.5, 0.5, 0.5)));
    // 地表材质：散射光材质，棋盘纹理
    auto ground_material2 = make_shared<Lambertian>(make_shared<CheckerTexture>(RGBColor(0.2, 0.3, 0.1), RGBColor(0.9, 0.9, 0.9)));
    // 添加一个特别大的球作为地表
    world.add(make_shared<Sphere>(Point3(0, -1000, 0), 1000, ground_material2));

    // 创建最多22*22个随机小球，即最多484个随机小球
    int minRandom = -11, maxRandom = 11;
    for (int a = minRandom; a < maxRandom; a++) {
        for (int b = minRandom; b < maxRandom; b++) {
            // 随机一个选择材质的浮点数，主要用于随机不同材质的球的概率
            auto choose_mat = random_double();
            // 随机当前小球的中心位置
            Point3 center(a + 0.9 * random_double(), 0.2, b + 0.9 * random_double());

            // 如果中心位置不太靠世界坐标原点左右
            if ((center - Point3(4, 0.2, 0)).length() > 0.9) {
                shared_ptr<Material> sphere_material;

                // 如果随机材质浮点数小于0.8，即创建散射材质，即80%的概率 
                if (choose_mat < 0.8) {
                    // diffuse
                    auto albedo = RGBColor::randomVec() * RGBColor::randomVec();
                    sphere_material = make_shared<Lambertian>(albedo);

                    auto center2 = center + Vec3(0, random_double(0,.5), 0);
                    world.add(make_shared<MovingSphere>(center,center2, minTime, maxTime,0.2,sphere_material));
                }
                // 如果随机材质浮点数小于0.95，即创建金属材质，即15%的概率 
                else if (choose_mat < 0.95) {
                    // metal
                    auto albedo = RGBColor::randomVec(0.5, 1);
                    auto fuzz = random_double(0, 0.5);
                    sphere_material = make_shared<Metal>(albedo, fuzz);
                    world.add(make_shared<Sphere>(center, 0.2, sphere_material));
                }
                // 剩下的5%的概率创建玻璃球 
                else {
                    // glass
                    sphere_material = make_shared<Dielectric>(1.5);
                    world.add(make_shared<Sphere>(center, 0.2, sphere_material));
                }
            }
        }
    }

    // 玻璃大球
    auto material1 = make_shared<Dielectric>(1.5);
    world.add(make_shared<Sphere>(Point3(0, 1, 0), 1.0, material1));

    // 散射大球
    auto material2 = make_shared<Lambertian>(RGBColor(0.4, 0.2, 0.1));
    world.add(make_shared<Sphere>(Point3(-4, 1, 0), 1.0, material2));

    // 金属大球
    auto material3 = make_shared<Metal>(RGBColor(0.7, 0.6, 0.5), 0.0);
    world.add(make_shared<Sphere>(Point3(4, 1, 0), 1.0, material3));

    return world;
}


// 创建一个随机场景
Scene random_scene_test() {
    Scene world;

    // 地表材质：散射光材质，灰色
    auto ground_material = make_shared<Lambertian>(RGBColor(0.5, 0.5, 0.5));
    // 添加一个特别大的球作为地表
    world.add(make_shared<Sphere>(Point3(0, -1000, 0), 1000, ground_material));

    // diffuse
    auto albedo = RGBColor::randomVec() * RGBColor::randomVec();
    auto sphere_material = make_shared<Lambertian>(albedo);

    Point3 center(0.9 * random_double(), 0.2, 0.9 * random_double());
    auto center2 = center + Vec3(0, random_double(0, .5), 0);
    world.add(make_shared<MovingSphere>(center, center2, minTime, maxTime, 0.2, sphere_material));

    return world;
}

// 纹理测试场景
Scene two_spheres_scene() {
    Scene world;

    // 材质：散射光材质，棋盘纹理
    auto ground_material = make_shared<Lambertian>(make_shared<CheckerTexture>(RGBColor(0.2, 0.3, 0.1), RGBColor(0.9, 0.9, 0.9)));
    
    //两个大球
    world.add(std::make_shared<Sphere>(Point3(0, -10, 0), 10, ground_material));
    world.add(std::make_shared<Sphere>(Point3(0, 10, 0), 10, ground_material));

    


    return world;
}


Scene two_perlin_spheres() {
    Scene objects;

    auto pertext = std::make_shared<NoiseTextureVec>(4);
    objects.add(std::make_shared<Sphere>(Point3(0, -1000, 0), 1000, std::make_shared<Lambertian>(pertext)));
    objects.add(std::make_shared<Sphere>(Point3(0, 2, 0), 2, std::make_shared<Lambertian>(pertext)));

    return objects;
}


Scene earth() {
    auto earth_texture = std::make_shared<ImageTexture>("../image/earthmap.jpg");
    auto earth_surface = std::make_shared<Lambertian>(earth_texture);
    auto globe = std::make_shared<Sphere>(Point3(0, 0, 0), 2, earth_surface);

    return Scene(globe);
}



