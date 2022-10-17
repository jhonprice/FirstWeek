#pragma once

#define STB_IMAGE_WRITE_IMPLEMENTATION
#define _CRT_SECURE_NO_WARNINGS
#include <vector>
#include "vec3.h"
#include<memory>
#include "../include/stb_image_write.h"

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
    void writeTgaFile(const std::string& s);
};



void Film::setPix(int x, int y, RGBColor color) {

    auto r = color.x() * maxColorParam;
    auto g = color.y() * maxColorParam;
    auto b = color.z() * maxColorParam;


    result[(x * imageCW + y) * 3 + 0] = static_cast<int>(r);
    result[(x * imageCW + y) * 3 + 1] = static_cast<int>(g);
    result[(x * imageCW + y) * 3 + 2] = static_cast<int>(b);
}


void Film::writeTgaFile(const std::string& s) {
    stbi_write_tga(s.c_str(), imageCW, imageCH, mode, result.data());
}