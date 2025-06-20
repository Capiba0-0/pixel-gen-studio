#pragma once

#include <SFML/Graphics.hpp>

#include <vector>

namespace PGS
{
class Canvas
{
private:
	sf::Vector2u m_size;

	std::vector<std::uint8_t> m_pixels;

	sf::Texture m_texture;
	sf::Sprite m_sprite;

public:
	// --- Constructors | Destructor ---
	Canvas(const sf::Vector2u& size);
	Canvas(unsigned int width, unsigned int height);
	~Canvas() = default;

	// --- Methods ---
	void setPixel(unsigned int x, unsigned int y, const sf::Color& color); 
	void setPixel(const sf::Vector2u& pos, const sf::Color& color)
	{
		setPixel(pos.x, pos.y, color);
	}

	void clear(const sf::Color& color);
	sf::Vector2u getSize() const;

	void updateTexture();

	void render(sf::RenderTarget& target, const sf::FloatRect& bounds, float factor) const;
};

} // namespace PGS