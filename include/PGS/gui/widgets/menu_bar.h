#pragma once

#include "PGS/gui/widget.h"
#include "PGS/gui/ui_context.h"

#include "imgui.h"
#include <SFML/Graphics/Texture.hpp>

namespace PGS::Gui {

class MenuBar final : public Widget
{
public:
    explicit MenuBar(const sf::Texture& icon);

    void renderContent(UIContext& context) override;

    void processInput(UIContext& context) override;

private:
    ImVec2 m_menuBarFramePadding;
    const sf::Texture& m_icon;

    static constexpr float MENU_BAR_PADDING_Y = 12.0f;
};

} // namespace PGS::gui

