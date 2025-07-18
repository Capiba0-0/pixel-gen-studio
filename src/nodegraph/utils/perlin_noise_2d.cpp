#include "PGS/nodegraph/utils/perlin_noise_2d.h"

#include <random>
#include <numeric>
#include <algorithm>
#include <cmath>

PGS::NodeGraph::PerlinNoise2D::PerlinNoise2D(const unsigned int seed) {
    permutation.resize(256);
    std::iota(permutation.begin(), permutation.end(), 0);

    std::default_random_engine engine(seed);
    std::shuffle(permutation.begin(), permutation.end(), engine);

    permutation.insert(permutation.end(), permutation.begin(), permutation.end());
}

float PGS::NodeGraph::PerlinNoise2D::getValue(const sf::Vector2f& pos) const {
    const int xi = static_cast<int>(std::floor(pos.x)) & 255;
    const int yi = static_cast<int>(std::floor(pos.y)) & 255;

    const auto xf = static_cast<float>(pos.x - std::floor(pos.x));
    const auto yf = static_cast<float>(pos.y - std::floor(pos.y));

    const float u = fade(xf);
    const float v = fade(yf);

    const int aa = hash(xi,     yi);
    const int ab = hash(xi,     yi + 1);
    const int ba = hash(xi + 1, yi);
    const int bb = hash(xi + 1, yi + 1);

    const float x1 = lerp(grad(aa, xf,     yf    ), grad(ab, xf,     yf - 1), v);
    const float x2 = lerp(grad(ba, xf - 1, yf    ), grad(bb, xf - 1, yf - 1), v);

    return lerp(x1, x2, u);
}

float PGS::NodeGraph::PerlinNoise2D::fade(const float t) const {
    return t * t * t * (t * (t * 6 - 15) + 10);
}

float PGS::NodeGraph::PerlinNoise2D::lerp(const float a, const float b, const float t) const {
    return a + t * (b - a);
}

float PGS::NodeGraph::PerlinNoise2D::grad(const int hash, const float x, const float y) const {
    int h = hash & 7;
    switch (h) {
        case 0: return  x + y;
        case 1: return -x + y;
        case 2: return  x - y;
        case 3: return -x - y;
        case 4: return  x;
        case 5: return -x;
        case 6: return  y;
        case 7: return -y;

        default: return 0;
    }
}

int PGS::NodeGraph::PerlinNoise2D::hash(const int x, const int y) const {
    return permutation[permutation[x & 255] + (y & 255)];
}