// Starter.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#define STB_IMAGE_WRITE_IMPLEMENTATION
#define _CRT_SECURE_NO_WARNINGS

#include <iostream>
#include <array>
#include <chrono>
#include "../include/stb_image_write.h"
using namespace std::chrono;

//Image的大小（像素
const int imageCW = 1280;
const int imageCH = 720;
const int length = imageCW * imageCH * 3;
std::array<unsigned char, length> result{ 0 };
const double maxColorParam = 255.999;

struct Color {
    int r;
    int g;
    int b;
};

void setPix(int x, int y, Color color) {
    result[(x * imageCW + y) * 3 + 0] = color.r;
    result[(x * imageCW + y) * 3 + 1] = color.g;
    result[(x * imageCW + y) * 3 + 2] = color.b;
}


int main()
{

    // Render：输出一张图片，列变化控制r分量，行变化控制g分量，b分量保持0.25
    // 默认从上到下，从左到右

    auto start = system_clock::now();
    for (int x{ 0 }; x < imageCH; x++) {
        std::cout << "\rScanlines remaining: " << x << ' ' << std::flush;
        for (int y{ 0 }; y < imageCW; y++) {
            auto r = double(y) / (imageCW - 1);
            auto g = double(x) / (imageCH - 1);
            auto b = 0.25;

            r *= maxColorParam;
            g *= maxColorParam;
            b *= maxColorParam;

            setPix(x, y, { static_cast<int>(r),static_cast<int>(g), static_cast<int>(b) });
  
        }
    }
    auto end = system_clock::now();
    auto duration = duration_cast<microseconds>(end - start);

    //写入文件
    // 默认从上到下，从左到右,现在翻转（00为左下角
    stbi_flip_vertically_on_write(1);
    stbi_write_tga("./test.tga", imageCW, imageCH, 3, result.data());

    //报告完成
    std::cout << "\nDone: "<< double(duration.count()) * microseconds::period::num / microseconds::period::den<<" s" << std::endl;
    return 0;
}
