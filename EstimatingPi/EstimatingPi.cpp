// EstimatingPi.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include <iomanip>
#include "../Starter/math_helper.h"

//面积概率：内切圆和正方形面积比理论上等于点落在各自区域的概率比
void estimate1() {
    int N = 1000;
    int inside_circle = 0;
    for (int i = 0; i < N; i++) {
        auto x = random_double(-1, 1);
        auto y = random_double(-1, 1);
        if (x * x + y * y < 1)
            inside_circle++;
    }
    std::cout << std::fixed << std::setprecision(12);
    std::cout << "Estimate of Pi = " << 4 * double(inside_circle) / N << '\n';
}

//一直随机采样：有收益递减的问题
void estimateForever() {
    int inside_circle = 0;
    int runs = 0;
    std::cout << std::fixed << std::setprecision(12);
    while (true) {
        runs++;
        auto x = random_double(-1, 1);
        auto y = random_double(-1, 1);
        if (x * x + y * y < 1)
            inside_circle++;

        if (runs % 10000000 == 0)
            std::cout << "Estimate of Pi = "
            << 4 * double(inside_circle) / runs
            << '\n';
    }
}

//分层采样：存在维度诅咒的问题
void estimateJittering() {
    int inside_circle = 0;
    int inside_circle_stratified = 0;
    int sqrt_N = 10000;
    for (int i = 0; i < sqrt_N; i++) {
        for (int j = 0; j < sqrt_N; j++) {
            // 先用普通随机的 -1 到 1
            auto x = random_double(-1, 1);
            auto y = random_double(-1, 1);
            // 这个是普通MC估算方法，只是用来对比
            if (x * x + y * y < 1)
                inside_circle++;
            // 这个是随机在 10000*10000的小格子内的坐标
            x = 2 * ((i + random_double()) / sqrt_N) - 1;
            y = 2 * ((j + random_double()) / sqrt_N) - 1;
            // 这个才是MC的分层估算方法
            if (x * x + y * y < 1)
                inside_circle_stratified++;
        }
    }

    auto N = static_cast<double>(sqrt_N) * sqrt_N;
    std::cout << std::fixed << std::setprecision(12);
    std::cout
        << "Regular    Estimate of Pi = "
        << 4 * double(inside_circle) / (sqrt_N * sqrt_N) << '\n'
        << "Stratified Estimate of Pi = "
        << 4 * double(inside_circle_stratified) / (sqrt_N * sqrt_N) << '\n';
}

int main() {
    estimateJittering();
}