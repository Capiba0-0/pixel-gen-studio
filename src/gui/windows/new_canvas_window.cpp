#include "PGS/gui/windows/new_canvas_window.h"

#include "PGS/core/config.h"
#include "PGS/gui/imgui_helpers.h"
#include "PGS/gui/ui_events.h"

#include "imgui.h"

using namespace PGS::gui;

NewCanvasWindow::NewCanvasWindow()
	: m_currentSelectionIndex{ m_config.defaultSizeIndex }
	, m_bgColor{ 
		static_cast<float>(m_config.color.r) / 255.0f,
		static_cast<float>(m_config.color.g) / 255.0f,
		static_cast<float>(m_config.color.b) / 255.0f,
		static_cast<float>(m_config.color.a) / 255.0f 
	}
{
	m_sizeItems.reserve(m_config.sizeOptions.size());
	for (const auto& size : m_config.sizeOptions)
	{
		m_sizeItems.push_back((std::to_string(size.x) + "x" + std::to_string(size.y)));
	}
}

void NewCanvasWindow::renderContent(UIContext& context)
{
	if (!m_isOpen)
		return;

	// Style configuration
	pusherStyleColor windowBg{ ImGuiCol_WindowBg, ImVec4{ 0.09f, 0.09f, 0.09f, 1.00f } };

	ImGui::SetNextWindowSize(ImVec2{ WINDOW_WIDTH, WINDOW_HEIGHT });
	const ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(ImVec2{ (viewport->WorkSize.x - WINDOW_WIDTH) / 2,
									(viewport->WorkSize.y - WINDOW_HEIGHT) / 2 });


	ImGui::Begin("Create new canvas", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize);

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
		PGS::events::NewCanvasRequest eventData;
		eventData.size = m_config.sizeOptions[m_currentSelectionIndex];
		eventData.bgColor = sf::Color(
			static_cast<uint8_t>(m_bgColor[0] * 255.0f),
			static_cast<uint8_t>(m_bgColor[1] * 255.0f),
			static_cast<uint8_t>(m_bgColor[2] * 255.0f),
			static_cast<uint8_t>(m_bgColor[3] * 255.0f)
		);

		context.newCanvasRequest = eventData;

		close();
	}
	ImGui::SameLine();
	if (ImGui::Button("Cancel")) {
		close();
	}

	ImGui::End();
}