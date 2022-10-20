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
Scene random_scene();
Scene random_scene_test();
Scene two_spheres_scene();
Scene two_perlin_spheres();
Scene earth();
Scene simple_light();
Scene cornell_box();
Scene cornell_smoke();
Scene final_scene();

//初始化最终图像
Film film{ 800,800,3};
//Film film{};
const int samples_per_pixel = 2;
const int max_depth = 50;
float cameraFov = 20.0;
const double minTime = 0.;
const double maxTime = 1.;



RGBColor ray_color(Ray& r,const Hittable& world, int depth, RGBColor bgColor) {
    Hit_record rec{};


    if (depth <= 0)
        return RGBColor(0, 0, 0);

    if (world.hit(r, rec)) {
        
        Ray scattered{};
        RGBColor attenuation{};
        RGBColor emitted = rec.mat_ptr->emitted(rec.uv.first, rec.uv.second, rec.p);

        if (rec.mat_ptr->scatter(r, rec, attenuation, scattered)) {
            return emitted + attenuation * ray_color(scattered, world, depth - 1, bgColor);
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
    RGBColor bgColor{ .7, .8, 1. };
    CameraFrame cFrame{ {13,2,3},{0,0,0},{0,1,0} };

    switch (8) {
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
        case 5:
            bgColor = RGBColor{ 0,0,0 };
            cFrame = { {26,3,6},{0,2,0},{0,1,0} };
            scene = simple_light();
            break;
        case 6:
            bgColor = RGBColor{ 0,0,0 };
            cFrame = { {278,278,-800},{278,278,0},{0,1,0} };
            cameraFov = 40.;
            scene = cornell_box();
            break;
        case 7:
            bgColor = RGBColor{ 0,0,0 };
            cFrame = { {278,278,-800},{278,278,0},{0,1,0} };
            cameraFov = 40.;
            scene = cornell_smoke();
            break;
        case 8:
            bgColor = RGBColor{ 0,0,0 };
            cFrame = { {478, 278, -600},{278, 278, 0},{0,1,0} };
            cameraFov = 40.;
            scene = final_scene();
            break;
        default:
            scene = random_scene_test();
            break;
    }

    
    // World
    Scene world{ scene };
    //BVH_NODE world{ { scene } ,minTime ,maxTime };


    //Camera camera{ {{-2,2,1},{0,0,-1},{0,1,0}}, cameraFov, film.getAspectRadio() };
    RealCamera camera{ cFrame, cameraFov, film.getAspectRadio(),0.,1.};


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
    //world.add(make_shared<MovingSphere>(center, center2, minTime, maxTime, 0.2, sphere_material));


    
    auto white = make_shared<Lambertian>(RGBColor(.73, .73, .73));
    std::shared_ptr<Hittable> box1 = std::make_shared<Box>(Point3(0, 0, 0), Point3(1, 1, 1), white);
    
    //world.add(box1);
    world.add(std::make_shared<Constant_medium>(box1, 0.01, RGBColor(0, 0, 0)));


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


Scene simple_light() {
    Scene objects;

    auto pertext = std::make_shared<NoiseTextureVec>(4);
    objects.add(std::make_shared<Sphere>(Point3(0, -1000, 0), 1000, std::make_shared<Lambertian>(pertext)));
    objects.add(std::make_shared<Sphere>(Point3(0, 2, 0), 2, std::make_shared<Lambertian>(pertext)));

    // 光源材质，注意这里的光源色大于(1,1,1)的白色，因为它要把周围照亮
    auto difflight = std::make_shared<DiffuseLight>(RGBColor(4, 4, 4));
    objects.add(std::make_shared<XY_Rect>(3, 5, 1, 3, -2, difflight));

    return objects;
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

Scene cornell_smoke() {
    Scene objects;

    auto red = make_shared<Lambertian>(RGBColor(.65, .05, .05));
    auto white = make_shared<Lambertian>(RGBColor(.73, .73, .73));
    auto green = make_shared<Lambertian>(RGBColor(.12, .45, .15));
    auto light = make_shared<DiffuseLight>(RGBColor(7, 7, 7));

    objects.add(make_shared<YZ_Rect>(0, 555, 0, 555, 555, green));
    objects.add(make_shared<YZ_Rect>(0, 555, 0, 555, 0, red));
    objects.add(make_shared<XZ_Rect>(113, 443, 127, 432, 554, light));
    objects.add(make_shared<XZ_Rect>(0, 555, 0, 555, 0, white));
    objects.add(make_shared<XZ_Rect>(0, 555, 0, 555, 555, white));
    objects.add(make_shared<XY_Rect>(0, 555, 0, 555, 555, white));


    //objects.add(std::make_shared<Box>(Point3(130, 0, 65), Point3(295, 165, 230), white));
    //objects.add(std::make_shared<Box>(Point3(265, 0, 295), Point3(430, 330, 460), white));


    std::shared_ptr<Hittable> box1 = std::make_shared<Box>(Point3(0, 0, 0), Point3(165, 330, 165), white);
    box1 = std::make_shared<Rotate_Y>(box1, 15);
    box1 = std::make_shared<Translate>(box1, Vec3(265, 0, 295));

   

    std::shared_ptr<Hittable> box2 = std::make_shared<Box>(Point3(0, 0, 0), Point3(165, 165, 165), white);
    box2 = std::make_shared<Rotate_Y>(box2, -18);
    box2 = std::make_shared<Translate>(box2, Vec3(130, 0, 65));


    objects.add(std::make_shared<Constant_medium>(box1, 0.01, RGBColor(0, 0, 0)));
    objects.add(std::make_shared<Constant_medium>(box2, 0.01, RGBColor(1, 1, 1)));

    return objects;
}


// next week最终测试场景
Scene final_scene()
{
    Scene boxes1;
    // 地表方块材质：淡绿色纯色纹理
    auto ground = make_shared<Lambertian>(RGBColor(0.48, 0.83, 0.53));

    // x 和 y方向各20个方块
    const int boxes_per_side = 20;

    // 对于这20*20个地表方块
    for (int i = 0; i < boxes_per_side; i++)
    {
        for (int j = 0; j < boxes_per_side; j++)
        {
            auto w = 100.0;
            auto x0 = -1000.0 + i * w;
            auto z0 = -1000.0 + j * w;
            auto y0 = 0.0;
            auto x1 = x0 + w;
            auto y1 = random_double(1, 101);
            auto z1 = z0 + w;

            // 创建地表方块，并添加到方块列表
            boxes1.add(make_shared<Box>(Point3(x0, y0, z0), Point3(x1, y1, z1), ground));
        }
    }

    Scene objects;

    // 20*20个地表方块使用BVH层次包围体来划分，以便提速
    objects.add(make_shared<BVH_NODE>(boxes1, 0, 1));

    // 光源材质
    auto light = make_shared<DiffuseLight>(RGBColor(7, 7, 7));
    // 在场景上方添加一个矩形光源
    objects.add(make_shared<XZ_Rect>(123, 423, 147, 412, 554, light));

    // 棕色运动中的球
    auto center1 = Point3(400, 400, 200);
    auto center2 = center1 + Vec3(30, 0, 0);
    auto moving_sphere_material = make_shared<Lambertian>(RGBColor(0.7, 0.3, 0.1));
    objects.add(make_shared<MovingSphere>(center1, center2, 0, 1, 50, moving_sphere_material));

    // 玻璃球
    objects.add(make_shared<Sphere>(Point3(260, 150, 45), 50, make_shared<Dielectric>(1.5)));

    // 浅灰色金属球
    objects.add(make_shared<Sphere>(
        Point3(0, 150, 145), 50, make_shared<Metal>(RGBColor(0.8, 0.8, 0.9), 1.0)
        ));


    // 蓝色高密度体积球体，它的包围体是一个玻璃球
    auto boundary = make_shared<Sphere>(Point3(360, 150, 145), 70, make_shared<Dielectric>(1.5));
    objects.add(boundary);
    // 包围体里面的体积体的密度比较大，颜色是蓝色
    objects.add(make_shared<Constant_medium>(boundary, 0.2, RGBColor(0.2, 0.4, 0.9)));

    // 全局白色雾气
    boundary = make_shared<Sphere>(Point3(0, 0, 0), 5000, make_shared<Dielectric>(1.5));
    objects.add(make_shared<Constant_medium>(boundary, .0001, RGBColor(1, 1, 1)));

    // 地球模型
    auto emat = make_shared<Lambertian>(make_shared<ImageTexture>("../image/earthmap.jpg"));
    objects.add(make_shared<Sphere>(Point3(400, 200, 400), 100, emat));

    // 带噪点的浅灰色金属球
    auto pertext = make_shared<NoiseTexture>(0.1);
    objects.add(make_shared<Sphere>(Point3(220, 280, 300), 80, make_shared<Lambertian>(pertext)));

    Scene boxes2;
    auto white = make_shared<Lambertian>(RGBColor(.73, .73, .73));

    // 1000个随机白色小球组成一个方块
    int ns = 1000;

    for (int j = 0; j < ns; j++)
    {
        boxes2.add(make_shared<Sphere>(Point3::randomVec(0, 165), 10, white));
    }

    objects.add(make_shared<Translate>(
        make_shared<Rotate_Y>(
            make_shared<BVH_NODE>(boxes2, 0.0, 1.0), 15),
        Vec3(-100, 270, 395)
        )
    );

    return objects;
}