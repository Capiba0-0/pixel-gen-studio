#include "PGS/core/buffers/grayscale_buffer.h"

#ifndef NDEBUG
#include <stdexcept>
#endif

PGS::GrayscaleBuffer::GrayscaleBuffer(const sf::Vector2u& size)
    : m_size(size)
{
    m_values.resize(m_size.x * m_size.y);
}


void PGS::GrayscaleBuffer::setValue(const sf::Vector2u& pos, const uint8_t value)
{
#ifndef NDEBUG
    if (pos.x >= m_size.x || pos.y >= m_size.y)
        throw std::invalid_argument("X must be in [0, " + std::to_string(m_size.x - 1) + "] and " +
            "Y must be in [0, " + std::to_string(m_size.y - 1) + "]");
#endif

    const unsigned int index = pos.y * m_size.x + pos.x;

    m_values[index] = value;
}

uint8_t PGS::GrayscaleBuffer::getValue(const sf::Vector2u& pos) const
{
#ifndef NDEBUG
    if (pos.x >= m_size.x || pos.y >= m_size.y)
        throw std::invalid_argument("X must be in [0, " + std::to_string(m_size.x - 1) + "] and " +
            "Y must be in [0, " + std::to_string(m_size.y - 1) + "]");
#endif

    const unsigned int index = pos.y * m_size.x + pos.x;

    return m_values[index];
}

sf::Vector2u PGS::GrayscaleBuffer::getSize() const
{
    return m_size;
}

const uint8_t* PGS::GrayscaleBuffer::getData() const
{
    return m_values.data();
}
