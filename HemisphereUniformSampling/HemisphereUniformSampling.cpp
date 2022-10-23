// HemisphereUniformSampling.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include "../Starter/math_helper.h"
#include <iomanip>
#include "..\Starter\vec3.h"

void hemisphereUni() {
    int N = 1000000;
    auto sum = 0.0;
    for (int i = 0; i < N; i++) {
        auto r1 = random_double();
        auto r2 = random_double();
        auto x = cos(2 * pi * r1) * 2 * sqrt(r2 * (1 - r2));
        auto y = sin(2 * pi * r1) * 2 * sqrt(r2 * (1 - r2));
        auto z = 1 - r2;
        sum += z * z * z / (1.0 / (2.0 * pi));
    }
    std::cout << std::fixed << std::setprecision(12);
    std::cout << "Pi/2     = " << pi / 2 << '\n';
    std::cout << "Estimate = " << sum / N << '\n';
}




void hemisphereDensity() {
    int N = 1000000;

    auto sum = 0.0;
    for (int i = 0; i < N; i++) {
        auto v = random_cosine_direction();
        sum += v.z() * v.z() * v.z() / (v.z() / pi);
    }

    std::cout << std::fixed << std::setprecision(12);
    std::cout << "Pi/2     = " << pi / 2 << '\n';
    std::cout << "Estimate = " << sum / N << '\n';
}
int main() {
    hemisphereUni();
    hemisphereDensity();
}
