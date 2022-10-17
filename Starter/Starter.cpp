// Starter.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#define STB_IMAGE_WRITE_IMPLEMENTATION
#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <array>
#include <chrono>
#include "image.h"
using namespace std::chrono;

//初始化最终图像
Film film;


int main()
{
    // Render：输出一张图片，列变化控制r分量，行变化控制g分量，b分量保持0.25
    const int imageCH = film.imageCH;
    const int imageCW = film.imageCW;


    //渲染循环
    auto start = system_clock::now();
    for (int x{ 0 }; x < imageCH; x++) {
        std::cout << "\rScanlines remaining: " << x << ' ' << std::flush;
        for (int y{ 0 }; y < imageCW; y++) {
            auto r = double(y) / (imageCW - 1);
            auto g = double(x) / (imageCH - 1);
            auto b = 0.25;

            film.setPix(x, y, { r,g,b });

  
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
