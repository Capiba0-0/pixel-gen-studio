#pragma once

#include <cstdint>
#include <SFML/Graphics/Color.hpp>

namespace PGS::NodeGraph::Utils
{
inline uint8_t lerp(const uint8_t valueA, const uint8_t valueB, const float t) {
    return static_cast<uint8_t>(static_cast<float>(valueA) * (1.0f - t) + static_cast<float>(valueB) * t);
}

inline sf::Color lerpColor(const sf::Color& colorA, const sf::Color& colorB, const float t) {
    return {
        lerp(colorA.r, colorB.r, t),
        lerp(colorA.g, colorB.g, t),
        lerp(colorA.b, colorB.b, t),
        lerp(colorA.a, colorB.a, t)
    };
}
} // namespace PGS::NodeGraph::Utils
