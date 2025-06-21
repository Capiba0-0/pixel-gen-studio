#include "PGS/gui/imgui_setup.h"

#include "imgui-SFML.h"
#include "imgui.h"

#include <stdexcept>

#include "PGS/gui/canvas.h"

void PGS::gui::setFonts()
{
    ImGuiIO& io = ImGui::GetIO();
    ImFont* mainFont = io.Fonts->AddFontFromFileTTF("assets/fonts/Inter-VariableFont.ttf", 15.0f);
    io.FontDefault = mainFont;

    if (!mainFont)
        throw std::runtime_error("Failed to load font: assets/fonts/Inter-VariableFont.ttf");

    if (!ImGui::SFML::UpdateFontTexture())
        throw std::runtime_error("Failed to update font texture for ImGui-SFML");
}

void PGS::gui::applyImguiStyle()
{
    ImGuiStyle& style = ImGui::GetStyle();

    // --- General ---
    // Main
    style.WindowPadding = ImVec2(8.0f, 8.0f);
    style.FramePadding = ImVec2(8.0f, 3.0f);
    style.ScrollbarSize = 10.0f;
    style.GrabMinSize = 13.0f;

    // Spacing
    style.ItemSpacing = ImVec2(8.0f, 4.0f);
    style.ItemInnerSpacing = ImVec2(4.0f, 4.0f);
    style.IndentSpacing = 7.0f;

    // Borders
    style.WindowBorderSize = 1.0f;
    style.ChildBorderSize = 1.0f;
    style.PopupBorderSize = 1.0f;
    style.FrameBorderSize = 0.0f;

    // Rounding
    style.WindowRounding = 7.0f;
    style.ChildRounding = 5.0f;
    style.FrameRounding = 5.0f;
    style.PopupRounding = 5.0f;
    style.GrabRounding = 4.0f;
    style.ScrollbarRounding = 11.0f;
    style.TabRounding = 5.0f;

    // Tabs
    style.TabBorderSize = 1.0f;
    style.TabBarBorderSize = 2.0f;
    style.TabCloseButtonMinWidthSelected = -1;
    style.TabCloseButtonMinWidthUnselected = 0;

    // Tables
    style.CellPadding = ImVec2(4.0f, 2.0f);

    // Windows
    style.WindowMinSize = ImVec2(175.0f, 75.0f);
    style.WindowTitleAlign = ImVec2(0.0f, 0.5f);
    style.WindowMenuButtonPosition = ImGuiDir_Left;

    // Widgets
    style.ColorButtonPosition = ImGuiDir_Right;
    style.ButtonTextAlign = ImVec2(0.5f, 0.5f);
    style.SelectableTextAlign = ImVec2(0.0f, 0.0f);
    style.SeparatorTextBorderSize = 2;
    style.SeparatorTextAlign = ImVec2(0.0f, 0.5f);
    style.LogSliderDeadzone = 4.0f;

    // Misc
    style.DisplayWindowPadding = ImVec2(19.0f, 19.0f);
    style.DisplaySafeAreaPadding = ImVec2(3.0f, 3.0f);

    // AntiAlias
    style.AntiAliasedLines = true;
    style.AntiAliasedLinesUseTex = true;
    style.AntiAliasedFill = true;

    // --- Colors ---
    // Text
    style.Colors[ImGuiCol_Text] = ImVec4(0.90f, 0.90f, 0.90f, 1.00f);
    style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);

    // Windows BG
    style.Colors[ImGuiCol_WindowBg] = ImVec4(0.18f, 0.18f, 0.18f, 1.00f);
    style.Colors[ImGuiCol_ChildBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    style.Colors[ImGuiCol_PopupBg] = ImVec4(0.09f, 0.09f, 0.09f, 1.00f);

    // Border
    style.Colors[ImGuiCol_Border] = ImVec4(0.22f, 0.22f, 0.22f, 0.32f);
    style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);

    // Frame
    style.Colors[ImGuiCol_FrameBg] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
    style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
    style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.28f, 0.45f, 0.70f, 1.00f);

    // Title
    style.Colors[ImGuiCol_TitleBg] = ImVec4(0.16f, 0.16f, 0.16f, 1.00f);
    style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.17f, 0.17f, 0.17f, 1.00f);
    style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.16f, 0.16f, 0.16f, 1.00f);

    // Menu Bar
    style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.15f, 0.15f, 0.15f, 1.00f);

    // Scrollbar
    style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    style.Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.25f, 0.25f, 0.25f, 1.00f);
    style.Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.33f, 0.33f, 0.33f, 1.00f);
    style.Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.38f, 0.38f, 0.38f, 1.00f);

    // Check Mark
    style.Colors[ImGuiCol_CheckMark] = ImVec4(0.69f, 0.69f, 0.69f, 1.00f);

    // Slider Grab
    style.Colors[ImGuiCol_SliderGrab] = ImVec4(1.00f, 1.00f, 1.00f, 0.24f);
    style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.74f, 0.74f, 0.74f, 0.56f);

    // Button
    style.Colors[ImGuiCol_Button] = ImVec4(0.33f, 0.33f, 0.33f, 1.00f);
    style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.40f, 0.40f, 0.40f, 1.00f);
    style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.28f, 0.45f, 0.70f, 1.00f);

    // Header
    style.Colors[ImGuiCol_Header] = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
    style.Colors[ImGuiCol_HeaderHovered] = ImVec4(0.26f, 0.26f, 0.26f, 1.00f);
    style.Colors[ImGuiCol_HeaderActive] = ImVec4(0.30f, 0.30f, 0.30f, 1.00f);

    // Separator
    style.Colors[ImGuiCol_Separator] = ImVec4(0.27f, 0.27f, 0.27f, 1.00f);
    style.Colors[ImGuiCol_SeparatorHovered] = ImVec4(0.33f, 0.33f, 0.33f, 1.00f);
    style.Colors[ImGuiCol_SeparatorActive] = ImVec4(0.37f, 0.37f, 0.37f, 1.00f);

    // Resize Grip
    style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.21f, 0.21f, 0.21f, 1.00f);
    style.Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.28f, 0.28f, 0.28f, 1.00f);
    style.Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.34f, 0.34f, 0.34f, 1.00f);

    // Tab
    style.Colors[ImGuiCol_TabHovered] = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
    style.Colors[ImGuiCol_Tab] = ImVec4(0.24f, 0.24f, 0.24f, 1.00f);
    style.Colors[ImGuiCol_TabSelected] = ImVec4(0.35f, 0.35f, 0.35f, 1.00f);
    style.Colors[ImGuiCol_TabSelectedOverline] = ImVec4(0.63f, 0.63f, 0.63f, 0.80f);
    style.Colors[ImGuiCol_TabDimmed] = ImVec4(0.43f, 0.43f, 0.43f, 0.82f);
    style.Colors[ImGuiCol_TabDimmedSelected] = ImVec4(0.51f, 0.51f, 0.51f, 0.84f);
    style.Colors[ImGuiCol_TabDimmedSelectedOverline] = ImVec4(0.86f, 0.86f, 0.86f, 0.00f);

    // Plot Lines
    style.Colors[ImGuiCol_PlotLines] = ImVec4(0.88f, 0.88f, 0.88f, 1.00f);
    style.Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.28f, 0.45f, 0.70f, 1.00f);
    style.Colors[ImGuiCol_PlotHistogram] = ImVec4(0.28f, 0.45f, 0.70f, 1.00f);
    style.Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.16f, 0.58f, 0.83f, 1.00f);

    // Table
    style.Colors[ImGuiCol_TableHeaderBg] = ImVec4(0.28f, 0.28f, 0.28f, 1.00f);
    style.Colors[ImGuiCol_TableBorderStrong] = ImVec4(0.39f, 0.39f, 0.39f, 1.00f);
    style.Colors[ImGuiCol_TableBorderLight] = ImVec4(0.30f, 0.30f, 0.30f, 1.00f);
    style.Colors[ImGuiCol_TableRowBg] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
    style.Colors[ImGuiCol_TableRowBgAlt] = ImVec4(1.00f, 1.00f, 1.00f, 0.07f);

    // Text
    style.Colors[ImGuiCol_TextLink] = ImVec4(0.67f, 0.67f, 0.67f, 0.80f);
    style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.69f, 0.69f, 0.69f, 1.00f);

    // Drag/Drop Target
    style.Colors[ImGuiCol_DragDropTarget] = ImVec4(0.24f, 0.64f, 1.00f, 1.00f);

    // Nav Highlight
    style.Colors[ImGuiCol_NavCursor] = ImVec4(0.65f, 0.65f, 0.65f, 0.80f);
    style.Colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.52f);
    style.Colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);

    // Modal Window Dimmed Bg 
    style.Colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.20f, 0.20f, 0.20f, 0.35f);
}