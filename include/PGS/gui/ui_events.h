#pragma once

#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/Color.hpp>

#include <variant>

namespace PGS::gui
{
	class Widget;
}

namespace PGS::events
{
struct NewCanvasRequest
{
	sf::Vector2u size;
	sf::Color bgColor;
};

struct CloseWidget
{
	gui::Widget* targetWidget;
};

struct RequestFocus
{
	gui::Widget* targetWidget;
};

struct RequestModal
{
	gui::Widget* targetWidget;
};

// UIEvent
// - Note: it must be in the end of this file.
using UIEvent = std::variant<
	NewCanvasRequest,
	CloseWidget,
	RequestFocus,
	RequestModal
>;

} // namespace PGS::events