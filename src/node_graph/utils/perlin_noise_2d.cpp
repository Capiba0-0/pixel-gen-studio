#include "PGS/node_graph/utils/perlin_noise_2d.h"

#include <random>
#include <numeric>
#include <algorithm>
#include <cmath>

PGS::NodeGraph::Utils::PerlinNoise2D::PerlinNoise2D(const unsigned int seed) {
    permutation.resize(256);
    std::iota(permutation.begin(), permutation.end(), 0);

    std::default_random_engine engine(seed);
    std::shuffle(permutation.begin(), permutation.end(), engine);

    permutation.insert(permutation.end(), permutation.begin(), permutation.end());
}

float PGS::NodeGraph::Utils::PerlinNoise2D::getValue(const sf::Vector2f& pos) const {
    const int xi = static_cast<int>(std::floor(pos.x)) & 255;
    const int yi = static_cast<int>(std::floor(pos.y)) & 255;

    const float xf = pos.x - std::floor(pos.x);
    const float yf = pos.y - std::floor(pos.y);

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

float PGS::NodeGraph::Utils::PerlinNoise2D::fade(const float t) {
    return t * t * t * (t * (t * 6 - 15) + 10);
}

float PGS::NodeGraph::Utils::PerlinNoise2D::lerp(const float a, const float b, const float t) {
    return a + t * (b - a);
}

float PGS::NodeGraph::Utils::PerlinNoise2D::grad(const int hash, const float x, const float y) {
    switch (hash & 7) {
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

int PGS::NodeGraph::Utils::PerlinNoise2D::hash(const int x, const int y) const {
    return permutation[permutation[x & 255] + (y & 255)];
}