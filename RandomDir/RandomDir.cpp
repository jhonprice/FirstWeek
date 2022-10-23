// RandomDir.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include "../Starter/math_helper.h"

void random_cosine_direction()
{
    double r1 = random_double();
    double r2 = random_double();
    double z = sqrt(1 - r2);
    double x = cos(2 * pi * r1) * 2 * sqrt(r2);
    double y = sin(2 * pi * r1) * 2 * sqrt(r2);
    std::cout << x << "," << y << "," << z << '\n';
}

void random_sphere() {
    auto r1 = random_double();
    auto r2 = random_double();
    auto x = cos(2 * pi * r1) * 2 * sqrt(r2 * (1 - r2));
    auto y = sin(2 * pi * r1) * 2 * sqrt(r2 * (1 - r2));
    auto z = 1 - 2 * r2;
    std::cout << x << "," << y << "," << z << '\n';
}

int main() {
    std::cout << 'A' << "," << 'B' << "," << 'C' << '\n';
    for (int i = 0; i < 200; i++) {
        //random_sphere();
        random_cosine_direction();
    }
}
