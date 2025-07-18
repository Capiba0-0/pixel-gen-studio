#pragma once

#include <vector>
#include <SFML/System/Vector2.hpp>

namespace PGS::NodeGraph
{

class PerlinNoise2D {
public:
    explicit PerlinNoise2D(unsigned int seed = 0);

    [[nodiscard]] float getValue(const sf::Vector2f& pos) const;

private:
    std::vector<int> permutation;

    [[nodiscard]] float fade(float t) const;
    [[nodiscard]] float lerp(float a, float b, float t) const;
    [[nodiscard]] float grad(int hash, float x, float y) const;
    [[nodiscard]] int hash(int x, int y) const;
};

} // namespace PGS::NodeGraph