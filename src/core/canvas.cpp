#include "PGS/core/canvas.h"

#include <cstdint>
#include <algorithm>
#include <stdexcept>

// --- Constructors ---
PGS::Canvas::Canvas(const sf::Vector2u& size)
	: m_size(size)
	, m_texture(m_size)
	, m_sprite(m_texture)
{
	m_pixels.resize(m_size.x * m_size.y * 4); // There are four numbers (r, g, b, a) for each pixel, so we multiply by 4
}

PGS::Canvas::Canvas(unsigned int width, unsigned int height)
	: m_size(width, height)
	, m_texture(m_size)
	, m_sprite(m_texture)
{
	m_pixels.resize(m_size.x * m_size.y * 4); // There are four numbers (r, g, b, a) for each pixel, so we multiply by 4
}


// --- Methods ---
void PGS::Canvas::setPixel(unsigned int x, unsigned int y, const sf::Color& color)
{
#ifndef NDEBUG
	if (x >= m_size.x || y >= m_size.y)
		throw std::invalid_argument("X must be in [0, " + std::to_string(m_size.x - 1) + "] and " +
			"Y must be in [0, " + std::to_string(m_size.y - 1) + "]");
#endif

	unsigned int index = (y * m_size.x + x) * 4;

	m_pixels[index] = color.r; // Set red color intensity
	m_pixels[index + 1] = color.g; // Set green color intensity
	m_pixels[index + 2] = color.b; // Set blue color intensity
	m_pixels[index + 3] = color.a; // Set alpha intensity
}

void PGS::Canvas::clear(const sf::Color& color)
{
	uint32_t packedColor = (static_cast<uint32_t>(color.a) << 24) |
						   (static_cast<uint32_t>(color.b) << 16) |
						   (static_cast<uint32_t>(color.g) << 8)  |
						   (static_cast<uint32_t>(color.r));

	uint32_t* pixels32 = reinterpret_cast<uint32_t*>(m_pixels.data());
	std::fill_n(pixels32, m_size.x * m_size.y, packedColor);
}

sf::Vector2u PGS::Canvas::getSize() const
{
	return m_size;
}


void PGS::Canvas::updateTexture()
{
	m_texture.update(m_pixels.data());
}

void PGS::Canvas::render(sf::RenderTarget& target, const sf::FloatRect& bounds, float factor) const
{
	float scaleX = bounds.size.x / m_size.x;
	float scaleY = bounds.size.y / m_size.y;
	float scale = std::min(scaleX, scaleY) / factor;

	sf::Sprite spriteToDraw = m_sprite;
	spriteToDraw.setScale(sf::Vector2f{ scale, scale });

	float finalWidth = m_size.x * scale;
	float finalHeight = m_size.y * scale;

	float posX = bounds.position.x + (bounds.size.x - finalWidth) / 2;
	float posY = bounds.position.y + (bounds.size.y - finalHeight) / 2;

	spriteToDraw.setPosition(sf::Vector2f{ posX, posY });
	target.draw(spriteToDraw);
}