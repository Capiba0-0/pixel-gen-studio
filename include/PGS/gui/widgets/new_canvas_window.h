#pragma once

#include "PGS/core/config.h"
#include "PGS/gui/ui_context.h"
#include "PGS/gui/widget.h"

#include <vector>
#include <string>

namespace PGS::gui
{

class NewCanvasWindow : public Widget
{
public:
	NewCanvasWindow();

	void renderContent(UIContext& context) override;

private:
	PGS::config::CanvasConfig m_config;
	int m_currentSelectionIndex;
	float m_bgColor[4]; 

	std::vector<std::string> m_sizeItems;

	static constexpr unsigned int WINDOW_WIDTH = 266;
	static constexpr unsigned int WINDOW_HEIGHT = 126;
};

} // namespace PGS::gui