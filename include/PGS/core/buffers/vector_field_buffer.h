#pragma once

#include <SFML/System/Vector2.hpp>

#include <vector>

namespace PGS
{

class VectorFieldBuffer
{
private:
    sf::Vector2u m_size;
    std::vector<sf::Vector2f> m_vectors;

public:
    // --- Constructors | Destructor ---
    explicit VectorFieldBuffer(const sf::Vector2u& size);
    ~VectorFieldBuffer() = default;

    // --- Methods ---
    void setVector(const sf::Vector2u& pos, const sf::Vector2f& vec);
    [[nodiscard]] sf::Vector2f getVector(const sf::Vector2u& pos) const;

    [[nodiscard]] sf::Vector2u getSize() const;
    [[nodiscard]] const sf::Vector2f* getData() const;
};

} // namespace PGS