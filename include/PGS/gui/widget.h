#pragma once

#include "PGS/gui/ui_context.h"

#include "imgui.h"
#include <SFML/Window/Event.hpp>
#include <SFML/System/Time.hpp>

namespace PGS::Gui
{

class Widget
{
public:
	// No-copyable
	Widget(const Widget&) = delete;
	Widget& operator=(const Widget&) = delete;

	Widget() = default;
	Widget(Widget&&) = default;
	Widget& operator=(Widget&&) = default;

	// Destructor
	virtual ~Widget() = default;

	virtual void renderContent(UIContext& context) = 0;

	virtual void update(const sf::Time deltaTime) { }
	virtual void onEvent(const sf::Event& event) { }

protected:
	virtual void processInput(UIContext& context)
	{
		if (ImGui::IsWindowHovered() &&
			ImGui::IsMouseClicked(ImGuiMouseButton_Left | ImGuiMouseButton_Middle | ImGuiMouseButton_Right))
		{
			context.emit(Events::RequestFocus{this});
		}
	}
};

} // namespace PGS::gui
