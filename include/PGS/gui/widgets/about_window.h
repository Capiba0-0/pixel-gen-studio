#pragma once

#include "PGS/gui/ui_context.h"
#include "PGS/gui/widget.h"

#include <SFML/Graphics/Texture.hpp>

namespace PGS::Gui
{

class AboutWindow final : public Widget
{
public:
	explicit AboutWindow(const sf::Texture& icon);

	void renderContent(UIContext& context) override;

private:
	static constexpr unsigned int WINDOW_WIDTH = 378;
	static constexpr unsigned int WINDOW_HEIGHT = 400;

	const sf::Texture& m_icon;
};

} // namespace PGS::gui