#pragma once

#define STB_IMAGE_WRITE_IMPLEMENTATION
#define _CRT_SECURE_NO_WARNINGS
#include <vector>
#include "vec3.h"
#include<memory>
#include "../include/stb_image_write.h"
#include "math_helper.h"

class Film {
public:
    Film(int w = 1280, int h = 720, int mode = 3) :imageCW(w), imageCH(h), mode(mode) {
        result.resize(length);
        stbi_flip_vertically_on_write(1);
    }
public:
    const int imageCW;
    const int imageCH;
    const int mode;
    const int length = imageCW * imageCW * mode;

    std::vector<unsigned char> result;
    const double maxColorParam = 255.999;
public:
    void setPix(int x, int y, RGBColor color);
    void setPix(int x, int y, RGBColor color, int samples_per_pixel);
    void writeTgaFile(const std::string& s);
    double getAspectRadio() { 
        return imageCW*1.f / imageCH; 
    }
};



void Film::setPix(int x, int y, RGBColor color) {

    auto r = color.x() * maxColorParam;
    auto g = color.y() * maxColorParam;
    auto b = color.z() * maxColorParam;


    result[(x * imageCW + y) * 3 + 0] = static_cast<int>(r);
    result[(x * imageCW + y) * 3 + 1] = static_cast<int>(g);
    result[(x * imageCW + y) * 3 + 2] = static_cast<int>(b);
}

void Film::setPix(int x, int y, RGBColor color, int samples_per_pixel) {

    auto scale = 1.0 / samples_per_pixel;
    auto tmpRGB = color * scale;


    result[(x * imageCW + y) * 3 + 0] = static_cast<int>(256 * clamp(tmpRGB.x(), 0.0, 0.999));
    result[(x * imageCW + y) * 3 + 1] = static_cast<int>(256 * clamp(tmpRGB.y(), 0.0, 0.999));
    result[(x * imageCW + y) * 3 + 2] = static_cast<int>(256 * clamp(tmpRGB.z(), 0.0, 0.999));
}


void Film::writeTgaFile(const std::string& s) {
    stbi_write_tga(s.c_str(), imageCW, imageCH, mode, result.data());
}