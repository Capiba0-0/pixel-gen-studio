#pragma once

#include <vector>
#include <SFML/System/Vector2.hpp>

namespace PGS::NodeGraph::Utils
{

class PerlinNoise2D {
public:
    explicit PerlinNoise2D(unsigned int seed = 0);

    [[nodiscard]] float getValue(const sf::Vector2f& pos) const;

private:
    std::vector<int> permutation;

    [[nodiscard]] static float fade(float t) ;
    [[nodiscard]] static float lerp(float a, float b, float t) ;
    [[nodiscard]] static float grad(int hash, float x, float y) ;
    [[nodiscard]] int hash(int x, int y) const;
};

} // namespace PGS::NodeGraph