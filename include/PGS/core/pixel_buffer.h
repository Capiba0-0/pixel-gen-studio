#pragma once

#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/Color.hpp>

#include <vector>
#include <cstdint>

namespace PGS
{
class PixelBuffer
{
private:
    sf::Vector2u m_size;
    std::vector<std::uint8_t> m_pixels;

public:
	// --- Constructors | Destructor ---
    explicit PixelBuffer(const sf::Vector2u& size);
    ~PixelBuffer() = default;

    // --- Methods ---
    void setPixel(const sf::Vector2u& pos, const sf::Color& color = sf::Color::White);
    [[nodiscard]] sf::Color getPixel(const sf::Vector2u& pos) const;

    void clear(const sf::Color& color = sf::Color::White);

    [[nodiscard]] sf::Vector2u getSize() const;
    [[nodiscard]] const uint8_t* getData() const;
};


} // namespace PGS