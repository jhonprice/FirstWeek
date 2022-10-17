#pragma once
#include <random>

// Utility Functions
const double pi = 3.1415926535897932385;
inline double degrees_to_radians(double degrees) {
    return degrees * pi / 180.0;
}

//Random

inline double random_double() {
    static std::uniform_real_distribution<double> distribution(0.0, 1.0);
    static std::mt19937 generator;
    return distribution(generator);
}


inline double random_double(double min, double max) {
    // Returns a random real in [min,max).
    return min + (max - min) * random_double();
}

// Ç¯ÖÆ
inline double clamp(double x, double min, double max) {
    if (x < min) return min;
    if (x > max) return max;
    return x;
}
