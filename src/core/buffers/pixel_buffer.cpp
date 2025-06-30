#include "PGS/core/buffers/pixel_buffer.h"

#include <cstdint>

#ifndef NDEBUG
#include <stdexcept>
#endif

PGS::PixelBuffer::PixelBuffer(const sf::Vector2u& size)
    : m_size(size)
{
    m_pixels.resize(m_size.x * m_size.y * 4); // There are four numbers (r, g, b, a) for each pixel, so we multiply by 4
}


void PGS::PixelBuffer::setPixel(const sf::Vector2u& pos, const sf::Color& color)
{
#ifndef NDEBUG
    if (pos.x >= m_size.x || pos.y >= m_size.y)
        throw std::invalid_argument("X must be in [0, " + std::to_string(m_size.x - 1) + "] and " +
            "Y must be in [0, " + std::to_string(m_size.y - 1) + "]");
#endif

    const unsigned int index = (pos.y * m_size.x + pos.x) * 4;

    m_pixels[index] = color.r; // Set red color intensity
    m_pixels[index + 1] = color.g; // Set green color intensity
    m_pixels[index + 2] = color.b; // Set blue color intensity
    m_pixels[index + 3] = color.a; // Set alpha intensity
}

sf::Color PGS::PixelBuffer::getPixel(const sf::Vector2u& pos) const
{
#ifndef NDEBUG
    if (pos.x >= m_size.x || pos.y >= m_size.y)
        throw std::invalid_argument("X must be in [0, " + std::to_string(m_size.x - 1) + "] and " +
            "Y must be in [0, " + std::to_string(m_size.y - 1) + "]");
#endif

    const unsigned int index = (pos.y * m_size.x + pos.x) * 4;

    return sf::Color{
        m_pixels[index],
        m_pixels[index + 1],
        m_pixels[index + 2],
        m_pixels[index + 3]
    };
}

void PGS::PixelBuffer::clear(const sf::Color& color) // TODO: This realization doesn't work at Big-endian architecture.
{                                                    //       Make it portable
    const uint32_t packedColor = (static_cast<uint32_t>(color.a) << 24) |
                           (static_cast<uint32_t>(color.b) << 16) |
                           (static_cast<uint32_t>(color.g) << 8)  |
                           (static_cast<uint32_t>(color.r));

    auto* pixels32 = reinterpret_cast<uint32_t*>(m_pixels.data());
    std::fill_n(pixels32, m_size.x * m_size.y, packedColor);
}

sf::Vector2u PGS::PixelBuffer::getSize() const
{
    return m_size;
}

const uint8_t* PGS::PixelBuffer::getData() const
{
    return m_pixels.data();
}