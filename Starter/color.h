#pragma once
#include "vec3.h"
#include "../MultipleObjectAndCodeClean/rtweekend.h"
#include <iostream>

//<<输出pmm像素颜色部分的抽象函数>>
void write_color(std::ostream& out, color pixel_color) {
    // Write the translated [0,255] value of each color component.
    out << static_cast<int>(255.999 * pixel_color.x()) << ' '
        << static_cast<int>(255.999 * pixel_color.y()) << ' '
        << static_cast<int>(255.999 * pixel_color.z()) << '\n';
}

//<<多样本 write_color,使用单个像素的多个样本来取平均进行反走样>>
void write_color(std::ostream& out, color pixel_color, int samples_per_pixel) {
    auto r = pixel_color.x();
    auto g = pixel_color.y();
    auto b = pixel_color.z();

    // Divide the color by the number of samples.
    auto scale = 1.0 / samples_per_pixel;
    r *= scale;
    g *= scale;
    b *= scale;

    // Write the translated [0,255] value of each color component.
    out << static_cast<int>(256 * clamp(r, 0.0, 0.999)) << ' '
        << static_cast<int>(256 * clamp(g, 0.0, 0.999)) << ' '
        << static_cast<int>(256 * clamp(b, 0.0, 0.999)) << '\n';
}
