#pragma once

#include "PGS/core/buffers/pixel_buffer.h"

#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Sprite.hpp>

#include <memory>

namespace PGS::Gui
{
class Canvas
{
private:
	sf::Texture m_texture;
	sf::Sprite m_sprite;

	std::shared_ptr<PixelBuffer> m_pixelBuffer;

	bool m_needsUpdate;

public:
	// --- Constructors | Destructor ---
	explicit Canvas(std::shared_ptr<PixelBuffer> pixelBuffer);
	~Canvas() = default;

	Canvas(const Canvas&) = delete;
	Canvas& operator=(const Canvas&) = delete;
	Canvas(Canvas&&) = default;
	Canvas& operator=(Canvas&&) = default;

	// --- Methods ---
	void setPixelBuffer(std::shared_ptr<PixelBuffer> pixelBuffer);

	void markForUpdate();
	void update();

	void render(sf::RenderTarget& target, const sf::FloatRect& bounds, float factor);
};

} // namespace PGS