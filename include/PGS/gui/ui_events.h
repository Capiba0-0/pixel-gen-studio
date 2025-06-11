#pragma once

#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/Color.hpp>

namespace PGS::events
{
struct NewCanvasRequest
{
	sf::Vector2u size;
	sf::Color bgColor;
};

} // namespace PGS::events