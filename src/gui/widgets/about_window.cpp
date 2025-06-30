#include "PGS/gui/widgets/about_window.h"

#include "PGS/gui/ui_helpers.h"

#include "imgui.h"
#include "imgui-SFML.h"

PGS::Gui::AboutWindow::AboutWindow(const sf::Texture& icon)
	: m_icon(icon)
{ }

void PGS::Gui::AboutWindow::renderContent(UIContext& context)
{
	// Style configuration
	PusherStyleColor windowBg{ ImGuiCol_WindowBg, ImVec4{ 0.09f, 0.09f, 0.09f, 1.00f } };

	ImGui::SetNextWindowSize(ImVec2{ WINDOW_WIDTH,WINDOW_HEIGHT });
	const ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(ImVec2{ (viewport->WorkSize.x - WINDOW_WIDTH) / 2,
							   (viewport->WorkSize.y - WINDOW_HEIGHT) / 2 });


	ImGui::Begin("About Window", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize);

	processInput(context);

	// Logo
	ImGui::Image(m_icon, sf::Vector2f{ 128, 128 }, sf::Color::White, sf::Color::Transparent);

	// Main info
	ImGui::Text("PixelGen Studio");
	ImGui::SameLine();
	ImGui::TextColored(ImVec4{ 0.50f, 0.50f, 0.50f, 1.00f }, "Version: 0.1.0");
	ImGui::Text("A procedural pixel graphics generator for creating unique ");
	ImGui::Text("textures, and patterns.");
	ImGui::Spacing();
	// Author info
	ImGui::Separator();
	ImGui::Text("Created by @Capiba0-0");
	ImGui::Text("GitHub: "); ImGui::SameLine();
	ImGui::TextLinkOpenURL("github.com/Capiba0-0", "https://github.com/Capiba0-0"); // TODO: Opening links does not work
	ImGui::Spacing();
	// Tech info
	ImGui::Separator();
	ImGui::Text("Powered by:");
	ImGui::BulletText("C++17");
	ImGui::BulletText("SFML:"); ImGui::SameLine(); ImGui::TextLinkOpenURL("www.sfml-dev.org", "https://www.sfml-dev.org");
	ImGui::BulletText("Dear ImGui:"); ImGui::SameLine(); ImGui::TextLinkOpenURL("github.com/ocornut/imgui", "https://github.com/ocornut/imgui");
	ImGui::Spacing();
	// License
	ImGui::Separator();
	ImGui::Text("This project is licensed under the MIT License.");
	ImGui::Spacing();


	// Close logic
	if (ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
		if (!ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow)) {
			context.emit(Events::CloseWidget{this});
		}
	}

	ImGui::End();
}