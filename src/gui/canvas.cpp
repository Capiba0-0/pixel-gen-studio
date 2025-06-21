#include "PGS/gui/canvas.h"

#include <SFML/Graphics/RenderTarget.hpp>

#include <algorithm>
#include <utility>

// --- Constructors ---
PGS::gui::Canvas::Canvas(std::shared_ptr<PixelBuffer> pixelBuffer)
	: m_sprite(m_texture)

	, m_needsUpdate(true)
{
	setPixelBuffer(std::move(pixelBuffer));
}

// --- Methods ---
void PGS::gui::Canvas::setPixelBuffer(std::shared_ptr<PixelBuffer> pixelBuffer)
{
	if (!pixelBuffer)
		throw std::invalid_argument("PixelBuffer is null");

	m_pixelBuffer = std::move(pixelBuffer);
	m_texture = sf::Texture{m_pixelBuffer->getSize()};

	m_sprite.setTextureRect(sf::IntRect{sf::Vector2i{0, 0},
		sf::Vector2i{static_cast<int>(m_texture.getSize().x),
							static_cast<int>(m_texture.getSize().y)}});

	markForUpdate();
}

void PGS::gui::Canvas::markForUpdate()
{
	m_needsUpdate = true;
}

void PGS::gui::Canvas::update()
{
	if (m_needsUpdate)
	{
		m_texture.update(m_pixelBuffer->getData());
		m_needsUpdate = false;
	}
}

void PGS::gui::Canvas::render(sf::RenderTarget& target, const sf::FloatRect& bounds, const float factor)
{
	const float scaleX = bounds.size.x / static_cast<float>(m_pixelBuffer->getSize().x);
	const float scaleY = bounds.size.y / static_cast<float>(m_pixelBuffer->getSize().y);
	const float scale = std::min(scaleX, scaleY) / factor;

	m_sprite.setScale(sf::Vector2f{ scale, scale });

	const float finalWidth = static_cast<float>(m_pixelBuffer->getSize().x) * scale;
	const float finalHeight = static_cast<float>(m_pixelBuffer->getSize().y) * scale;

	const float posX = bounds.position.x + (bounds.size.x - finalWidth) / 2;
	const float posY = bounds.position.y + (bounds.size.y - finalHeight) / 2;

	m_sprite.setPosition(sf::Vector2f{ posX, posY });
	target.draw(m_sprite);
}