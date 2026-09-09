//
// Created by ruairi on 21/08/2026.
//

#ifndef UTIL_HPP
#define UTIL_HPP
#include <cmath>
#include <random>
#include <vector>

inline std::vector<unsigned> generateSeeds(const unsigned int numberOfPaths) {
    std::random_device random_device;
    std::vector<unsigned> seeds;
    seeds.reserve(numberOfPaths);
    for (unsigned int i = 0; i < numberOfPaths; ++i) {
        seeds.push_back(random_device());
    }
    return seeds;
}

inline double zDistCDF(const double x) {
    constexpr double SQRT_HALF = 0.70710678118654752440;
    return 0.5 * std::erfc(-x * SQRT_HALF);
}
#endif //UTIL_HPP
