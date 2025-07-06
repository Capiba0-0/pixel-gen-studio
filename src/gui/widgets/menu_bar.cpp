#include "PGS/gui/widgets/menu_bar.h"

#include "PGS/gui/ui_helpers.h"

#include "imgui.h"
#include "imgui-SFML.h"
#include "PGS/gui/widgets/about_window.h"
#include "PGS/gui/widgets/about_window.h"
#include "PGS/gui/widgets/new_canvas_window.h"

PGS::Gui::MenuBar::MenuBar(const sf::Texture& icon)
    : m_menuBarFramePadding{ 0,0 }
    , m_icon{icon}
{ }

void PGS::Gui::MenuBar::processInput(UIContext& context)
{
    // Nothing for now -_-
}

void PGS::Gui::MenuBar::renderContent(UIContext& context)
{
    // Style config
    m_menuBarFramePadding = { ImGui::GetStyle().FramePadding.x, MENU_BAR_PADDING_Y };
    PusherStyleVar   framePadding(ImGuiStyleVar_FramePadding, m_menuBarFramePadding);
    PusherStyleColor button(ImGuiCol_Button, sf::Color{ 38, 38, 38, 255 });
    PusherStyleColor buttonActive(ImGuiCol_ButtonActive, sf::Color{ 76, 76, 76, 255 });
    PusherStyleColor buttonHovered(ImGuiCol_ButtonHovered, sf::Color{ 76, 76, 76, 255 });

    // Menu bar
    if (ImGui::BeginMainMenuBar()) {

        processInput(context);

        if (ImGui::ImageButton("About PixelGen Studio", m_icon, sf::Vector2f{ 18, 18 })) {
            context.uiManager.createWidget(typeid(AboutWindow));
        }

        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("New", "Ctrl+N")) {
                context.uiManager.createWidget(typeid(NewCanvasWindow));
            }
            // if (ImGui::MenuItem("Open", "Ctrl+O")) {}
            // if (ImGui::MenuItem("Save", "Ctrl+S")) {}
            if (ImGui::MenuItem("Quit", "Ctrl+Q"))
            {
                context.emit(Events::RequestQuit{});
            }

            ImGui::EndMenu();
        }

        // if (ImGui::BeginMenu("Edit")) {
        //     if (ImGui::MenuItem("Undo", "Ctrl+Z")) {}
        //     if (ImGui::MenuItem("Redo", "Ctrl+Y")) {}
        //
        //     ImGui::EndMenu();
        // }

        if (ImGui::BeginMenu("Window")) {

            ImGui::EndMenu();
        }
    }
    ImGui::EndMainMenuBar();

}

