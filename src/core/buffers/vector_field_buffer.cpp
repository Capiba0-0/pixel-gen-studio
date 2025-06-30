#include "PGS/core/buffers/vector_field_buffer.h"

#ifndef NDEBUG
#include <stdexcept>
#endif

PGS::VectorFieldBuffer::VectorFieldBuffer(const sf::Vector2u& size)
    : m_size(size)
{
    m_vectors.resize(size.x * size.y);
}


void PGS::VectorFieldBuffer::setVector(const sf::Vector2u& pos, const sf::Vector2f& vec)
{
#ifndef NDEBUG
    if (pos.x >= m_size.x || pos.y >= m_size.y)
        throw std::invalid_argument("X must be in [0, " + std::to_string(m_size.x - 1) + "] and " +
            "Y must be in [0, " + std::to_string(m_size.y - 1) + "]");
#endif

    const unsigned int index = pos.y * m_size.x + pos.x;

    m_vectors[index] = vec;
}

sf::Vector2f PGS::VectorFieldBuffer::getVector(const sf::Vector2u& pos) const
{
#ifndef NDEBUG
    if (pos.x >= m_size.x || pos.y >= m_size.y)
        throw std::invalid_argument("X must be in [0, " + std::to_string(m_size.x - 1) + "] and " +
            "Y must be in [0, " + std::to_string(m_size.y - 1) + "]");
#endif

    const unsigned int index = pos.y * m_size.x + pos.x;

    return m_vectors[index];
}

sf::Vector2u PGS::VectorFieldBuffer::getSize() const
{
    return m_size;
}

const sf::Vector2f* PGS::VectorFieldBuffer::getData() const
{
    return m_vectors.data();
}