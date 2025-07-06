#pragma once

#include "PGS/gui/ui_context.h"
#include "PGS/gui/node_editor/input/command.h"

namespace PGS::Gui
{
// Declaration
struct UIContext;
class NodeEditorState;
class NodeEditorInputHandler;

class NodeEditorRenderer
{
public:
    void renderNodes(const UIContext& context, const NodeEditorState& state);
    void renderLinks(const UIContext& context, const NodeEditorState& state);
    void renderPopups(const UIContext& context, const NodeEditorState& state, std::vector<std::unique_ptr<Command>>& commands);

private:
    // DRY Helpers
    void nodeCreationMenu(const UIContext& context, std::vector<std::unique_ptr<Command>>& commands);
};

} // namespace PGS::Gui