#pragma once
#ifndef PERLINNOISE_H
#define PERLINNOISE_H

#include <vector>
#include <cmath>
#include <algorithm>
#include <random>

class PerlinNoise {
public:
    PerlinNoise(unsigned int seed = 0);

    // Get Perlin noise value at coordinates x, y, z
    float noise(float x, float y, float z) const;

private:
    static const int PERMUTATION_SIZE = 256;
    std::vector<int> p;

    float fade(float t) const;
    float lerp(float t, float a, float b) const;
    float grad(int hash, float x, float y, float z) const;
};

#endif
