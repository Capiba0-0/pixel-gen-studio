#pragma once

#include "PGS/gui/ui_context.h"

#include <SFML/Window/Event.hpp>

namespace PGS::gui
{

class WindowPopup
{
protected:
	bool m_isOpen = false;

public:
	// Destructor
	virtual ~WindowPopup() = default;

	virtual bool isOpen() const { return m_isOpen; }
	virtual void open() { m_isOpen = true; }
	virtual void close() { m_isOpen = false; }

	virtual void renderContent(UIContext& context) = 0;

	virtual void update(float deltaTime) { }
	virtual void onEvent(const sf::Event& event) { }

protected:
	// No-copyable
	WindowPopup(const WindowPopup&) = delete;
	WindowPopup& operator=(const WindowPopup&) = delete;

	WindowPopup() = default;
	WindowPopup(WindowPopup&&) = default;
	WindowPopup& operator=(WindowPopup&&) = default;
};

} // namespace PGS::gui	