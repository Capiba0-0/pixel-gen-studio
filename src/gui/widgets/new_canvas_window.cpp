#include "PGS/gui/widgets/new_canvas_window.h"

#include "PGS/core/config.h"
#include "PGS/gui/ui_helpers.h"
#include "PGS/gui/ui_events.h"

#include "imgui.h"

PGS::Gui::NewCanvasWindow::NewCanvasWindow()
	: m_currentSelectionIndex{ m_config.DEFAULT_SIZE_INDEX }
	, m_bgColor{ 
		static_cast<float>(m_config.COLOR.r) / 255.0f,
		static_cast<float>(m_config.COLOR.g) / 255.0f,
		static_cast<float>(m_config.COLOR.b) / 255.0f,
		static_cast<float>(m_config.COLOR.a) / 255.0f 
	}
{
	m_sizeItems.reserve(m_config.sizeOptions.size());
	for (const auto& size : m_config.sizeOptions)
	{
		m_sizeItems.push_back((std::to_string(size.x) + "x" + std::to_string(size.y)));
	}
}

void PGS::Gui::NewCanvasWindow::renderContent(UIContext& context)
{
	// Style configuration
	PusherStyleColor windowBg{ ImGuiCol_WindowBg, ImVec4{ 0.09f, 0.09f, 0.09f, 1.00f } };

	ImGui::SetNextWindowSize(ImVec2{ WINDOW_WIDTH, WINDOW_HEIGHT });
	const ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(ImVec2{ (viewport->WorkSize.x - WINDOW_WIDTH) / 2,
									(viewport->WorkSize.y - WINDOW_HEIGHT) / 2 });


	ImGui::Begin("Create new canvas", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize);

	processInput(context);

	ImGui::Text("Create New Canvas");

	if (ImGui::BeginCombo("Canvas Size", m_sizeItems[m_currentSelectionIndex].c_str())) {
		for (int i = 0; i < m_sizeItems.size(); ++i) {
			const bool isSelected = (m_currentSelectionIndex == i);

			if (ImGui::Selectable(m_sizeItems[i].c_str(), isSelected)) {
				m_currentSelectionIndex = i;
			}
			if (isSelected) {
				ImGui::SetItemDefaultFocus();
			}
		}

		ImGui::EndCombo();
	}

	ImGui::ColorEdit4("Color", m_bgColor);
	ImGui::NewLine();

	if (ImGui::Button("Create")) {
		PGS::Events::NewCanvasRequest eventData;
		eventData.size = m_config.sizeOptions[m_currentSelectionIndex];
		eventData.bgColor = sf::Color(
			static_cast<uint8_t>(m_bgColor[0] * 255.0f),
			static_cast<uint8_t>(m_bgColor[1] * 255.0f),
			static_cast<uint8_t>(m_bgColor[2] * 255.0f),
			static_cast<uint8_t>(m_bgColor[3] * 255.0f)
		);

		context.emit(eventData);

		context.emit(Events::CloseWidget{this});
	}
	ImGui::SameLine();
	if (ImGui::Button("Cancel")) {
		context.emit(Events::CloseWidget{this});
	}

	ImGui::End();
}