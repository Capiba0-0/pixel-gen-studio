#include "PGS/gui/node_editor/input/node_editor_input_handler.h"

#include "PGS/gui/node_editor/input/commands.h"
#include "PGS/node_graph/types.h"

#include "imgui.h"
#include "imnodes.h"

void PGS::Gui::NodeEditorInputHandler::processInput(std::vector<std::unique_ptr<Command>>& commands)
{
    if (ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows))
    {
        handleKeyboardInput(commands);
        handleMouseInput(commands);
        handleImNodesEvents(commands);
    }
}

void PGS::Gui::NodeEditorInputHandler::handleKeyboardInput(std::vector<std::unique_ptr<Command>>& commands)
{
    if (ImGui::IsKeyPressed(ImGuiKey_X, false))
    {
        commands.push_back(std::make_unique<DeleteSelectedCommand>());
    }

    if (ImGui::IsKeyPressed(ImGuiKey_A) && ImGui::GetIO().KeyShift)
    {
        ImGui::OpenPopup("Node Creation Menu");
    }
}

void PGS::Gui::NodeEditorInputHandler::handleMouseInput(std::vector<std::unique_ptr<Command>>& commands)
{
    if (ImGui::IsMouseClicked(ImGuiMouseButton_Right) && ImNodes::NumSelectedNodes() == 0)
    {
        ImGui::OpenPopup("Node Editor Menu");
    }
}

void PGS::Gui::NodeEditorInputHandler::handleImNodesEvents(std::vector<std::unique_ptr<Command>>& commands)
{
    // Created link
    int startAttrCreated;
    int endAttrCreated;
    if (ImNodes::IsLinkCreated(&startAttrCreated, &endAttrCreated))
    {
        commands.push_back(std::make_unique<CreateLinkCommand>(startAttrCreated, endAttrCreated));
    }

    // Destroyed link
    int linkDestroyed;
    if (ImNodes::IsLinkDestroyed(&linkDestroyed))
    {
        commands.push_back(std::make_unique<DestroyLinkCommand>(linkDestroyed));
    }

    // Dropped link
    int startAttrDropped;
    if (ImNodes::IsLinkDropped(&startAttrDropped, true))
    {
        commands.push_back(std::make_unique<DropLinkCommand>(startAttrDropped));
    }
}