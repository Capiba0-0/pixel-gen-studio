#pragma once

#include "PGS/gui/ui_context.h"

#include "imgui.h"
#include <SFML/Window/Event.hpp>
#include <SFML/System/Time.hpp>

namespace PGS::gui
{

class Widget
{
protected:
	bool m_isVisible = true;

public:
	// No-copyable
	Widget(const Widget&) = delete;
	Widget& operator=(const Widget&) = delete;

	Widget() = default;
	Widget(Widget&&) = default;
	Widget& operator=(Widget&&) = default;

	// Destructor
	virtual ~Widget() = default;

	[[nodiscard]] virtual bool isVisible() const { return m_isVisible; }
	virtual void makeVisible() { m_isVisible = true; }
	virtual void makeInvisible() { m_isVisible = false; }

	virtual void renderContent(UIContext& context) = 0;

	virtual void update(sf::Time deltaTime) { }
	virtual void onEvent(const sf::Event& event) { }

protected:
	virtual void processInput(UIContext& context)
	{
		if (ImGui::IsWindowHovered() &&
			ImGui::IsMouseClicked(ImGuiMouseButton_Left | ImGuiMouseButton_Middle | ImGuiMouseButton_Right))
		{
			context.emit(events::RequestFocus{this});
		}
	}
};

} // namespace PGS::gui
