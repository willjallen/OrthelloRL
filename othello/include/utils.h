#ifndef UTILS_H
#define UTILS_H

#include <random>
#include <vector>

std::pair<int, int> sampleDistribution(const std::vector<float> &dist) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::discrete_distribution<int> d(dist.begin(), dist.end());
    int index = d(gen);
    return {index / 8, index % 8};
}

#endif