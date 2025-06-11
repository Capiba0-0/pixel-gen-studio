#pragma once

#include <optional>

#include "PGS/gui/ui_events.h"

namespace PGS::gui
{
struct UIContext
{
	std::optional<PGS::events::NewCanvasRequest> newCanvasRequest;
};

}