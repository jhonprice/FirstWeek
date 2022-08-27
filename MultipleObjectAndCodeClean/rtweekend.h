#pragma once
#include <cmath>
#include <limits>
#include <memory>
#include <random>

// Usings

using std::shared_ptr;
using std::make_shared;
using std::sqrt;

// Constants

const double infinity = std::numeric_limits<double>::infinity();
const double pi = 3.1415926535897932385;

// Utility Functions
#pragma region Utils
inline double degrees_to_radians(double degrees) {
    return degrees * pi / 180.0;
}
//钳制函数
inline double clamp(double x, double min, double max) {
    if (x < min) return min;
    if (x > max) return max;
    return x;
}
#pragma endregion


// Common Headers
#include "../Starter/color.h"
#include "../Starter/vec3.h"
#include "../RayCameraAndBackGround/ray.h"

//<<随机数部分>>
#pragma region RandomPart
inline double random_double() {
    // Returns a random real in [0,1).
    return rand() / (RAND_MAX + 1.0);
}

inline double random_double(double min, double max) {
    // Returns a random real in [min,max).
    return min + (max - min) * random_double();
}
//C++库的随机数
inline double random_double2() {
    static std::uniform_real_distribution<double> distribution(0.0, 1.0);
    static std::mt19937 generator;
    return distribution(generator);
}
#pragma endregion

