#pragma once

#include "PGS/gui/ui_events.h"
#include "PGS/gui/ui_manager_interface.h"

#include <optional>

namespace PGS::gui
{

struct UIContext
{
	// Events
	std::optional<events::NewCanvasRequest> newCanvasRequest = std::nullopt;

	std::optional<events::WidgetMouseClick> mouseButtonClicked = std::nullopt;

	// Managers
	UIManagerInterface& uiManager;
};

} // namespace PGS::gui