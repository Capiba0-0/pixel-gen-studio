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

	WidgetID m_id = 0;

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

	void setID(const WidgetID id) { m_id = id; }

protected:
	virtual void processInput(UIContext& context)
	{
		// Event processing
		if (!ImGui::IsWindowHovered())
			return;

		const ImGuiMouseButton mouse_buttons[] = {
			ImGuiMouseButton_Left,
			ImGuiMouseButton_Right,
			ImGuiMouseButton_Middle
		};
		const events::MouseButton pgs_buttons[] = {
			events::MouseButton::Left,
			events::MouseButton::Right,
			events::MouseButton::Middle
		};

		for (int i = 0; i < sizeof(mouse_buttons) / sizeof(mouse_buttons[0]); ++i)
		{
			if (ImGui::IsMouseClicked(mouse_buttons[i]))
			{
				context.mouseButtonClicked->targetWidgetID = m_id;
				context.mouseButtonClicked->button = pgs_buttons[i];
			}
		}
	}
};

} // namespace PGS::gui	