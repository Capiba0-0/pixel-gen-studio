#pragma once

#include "PGS/gui/types.h"

#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/Color.hpp>

namespace PGS::events
{
struct NewCanvasRequest
{
	sf::Vector2u size;
	sf::Color bgColor;
};

enum class MouseButton {
	Left,
	Right,
	Middle
};

struct WidgetMouseClick {
	gui::WidgetID targetWidgetID;
	MouseButton button;
};

} // namespace PGS::events