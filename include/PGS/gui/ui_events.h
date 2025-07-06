#pragma once

#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/Color.hpp>

#include <functional>
#include <memory>
#include <variant>

namespace PGS
{

namespace Gui
{
	class Widget;
}

class PixelBuffer;

} // namespace PGS


namespace PGS::Events
{

struct NewCanvasRequest
{
	sf::Vector2u size;
	sf::Color bgColor;
};

struct CloseWidget
{
	Gui::Widget* targetWidget;
};

struct RequestFocus
{
	Gui::Widget* targetWidget;
};

struct RequestModal
{
	Gui::Widget* targetWidget;
};

struct RequestQuit {};

// UIEvent
// - Note: it must be in the end of this file.
using UIEvent = std::variant<
	NewCanvasRequest,
	CloseWidget,
	RequestFocus,
	RequestModal,
	RequestQuit
>;

} // namespace PGS::events