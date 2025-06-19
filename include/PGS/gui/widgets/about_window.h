#pragma once

#include "PGS/gui/ui_context.h"
#include "PGS/gui/widget.h"

#include <SFML/Graphics/Texture.hpp>

namespace PGS::gui
{

class AboutWindow : public Widget
{
public:
	AboutWindow(const sf::Texture& icon);

	void renderContent(UIContext& context) override;

private:
	static constexpr unsigned int WINDOW_WIDTH = 354;
	static constexpr unsigned int WINDOW_HEIGHT = 362;

	const sf::Texture& m_icon;
};

} // namespace PGS::gui