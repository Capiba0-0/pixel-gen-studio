#pragma once

#include <SFML/System/Vector2.hpp>

#include <vector>
#include <cstdint>

namespace PGS
{

class GrayscaleBuffer
{
private:
    sf::Vector2u m_size;
    std::vector<std::uint8_t> m_values;

public:
    // --- Constructors | Destructor ---
    explicit GrayscaleBuffer(const sf::Vector2u& size);
    ~GrayscaleBuffer() = default;

    // --- Methods ---
    void setValue(const sf::Vector2u& pos, uint8_t value);
    [[nodiscard]] uint8_t getValue(const sf::Vector2u& pos) const;

    [[nodiscard]] sf::Vector2u getSize() const;
    [[nodiscard]] const uint8_t* getData() const;
};

} // namespace PGS