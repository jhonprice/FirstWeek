// Sphere_importance.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include "../Starter/vec3.h"
#include <iomanip>
inline double pdf(const Vec3& p) {
    return 1 / (4 * pi);
}

int main() {
    int N = 1000000;
    auto sum = 0.0;
    for (int i = 0; i < N; i++) {
        Vec3 d = random_unit_vector();
        auto cosine_squared = d.z() * d.z();
        sum += cosine_squared / pdf(d);
    }
    std::cout << std::fixed << std::setprecision(12);
    std::cout << "I = " << sum / N << '\n';
}
