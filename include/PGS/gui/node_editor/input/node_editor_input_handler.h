#pragma once

#include "PGS/gui/node_editor/input/command.h"

#include <memory>
#include <vector>

namespace PGS::Gui
{
// Declaration
struct UIContext;
class NodeEditorState;

class NodeEditorInputHandler
{
public:
    void processInput(std::vector<std::unique_ptr<Command>>& commands);

private:
    void handleKeyboardInput(std::vector<std::unique_ptr<Command>>& commands);
    void handleMouseInput(std::vector<std::unique_ptr<Command>>& commands);
    void handleImNodesEvents(std::vector<std::unique_ptr<Command>>& commands);
};

} // namespace PGS::Gui