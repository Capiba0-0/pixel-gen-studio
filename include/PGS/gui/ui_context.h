#pragma once

#include "PGS/gui/ui_events.h"
#include "PGS/core/managers/ui_manager_interface.h"

#include <SFML/System/Time.hpp>

#include <functional>

namespace PGS::gui
{
using EventEmitter = std::function<void(const events::UIEvent&)>;

struct UIContext
{
	// Events
	EventEmitter emit;

	sf::Time deltaTime;

	// Managers
	UIManagerInterface& uiManager;
};

} // namespace PGS::gui