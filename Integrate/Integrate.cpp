// Integrate.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include "../Starter/math_helper.h"
#include <iomanip>
//一维MC程序：估算x^2积分的平均高度（转化为长方形后

//采样N次，求平均
void sample1() {
    // 采样一百万个点
    int N = 1000000;
    auto sum = 0.0;
    // 每个点都是在0到2之间
    for (int i = 0; i < N; i++) {
        auto x = random_double(0, 2);
        sum += x * x;
    }
    std::cout << std::fixed << std::setprecision(12);
    // 通过一百万个点的高度和 除以 采样次数 即算出平均数，在乘以长度2，即算出面积了
    std::cout << "I = " << 2 * sum / N << '\n';
}


//作用1：pdf，2:非均匀采样的平均权重
inline double pdf(double x) {
    return 0.5 * x;
}
//非均匀采样
void sample2() {
    // 采样一百万个点
    int N = 1000000;
    auto sum = 0.0;
    // 数学推导注意：注意理解其中的数学逻辑（见book
    for (int i = 0; i < N; i++) {
        auto x = sqrt(random_double(0, 4));
        sum += x * x / pdf(x);
    }
    std::cout << std::fixed << std::setprecision(12);
    // 通过一百万个点的高度和 除以 采样次数 即算出平均数，在乘以长度2，即算出面积了
    std::cout << "I = " << sum / N << '\n';
}


inline double pdf2(double x) {
    return 0.5;
}

void sample3() {
    int N = 1000000;
    auto sum = 0.0;
    for (int i = 0; i < N; i++) {
        auto x = random_double(0, 2);
        sum += x * x / pdf2(x);
    }
    std::cout << std::fixed << std::setprecision(12);
    std::cout << "I = " << sum / N << '\n';
}


inline double pdf3(double x) {
    return 3 * x * x / 8;
}

void sample4() {
    int N = 1;
    auto sum = 0.0;
    for (int i = 0; i < N; i++) {
        auto x = pow(random_double(0, 8), 1. / 3.);
        sum += x * x / pdf3(x);
    }
    std::cout << std::fixed << std::setprecision(12);
    std::cout << "I = " << sum / N << '\n';
}

int main()
{
    sample4();
}
