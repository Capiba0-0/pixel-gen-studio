#pragma once

namespace PGS::Gui
{
struct UIContext;
class NodeEditorState;

class Command
{
public:
    virtual ~Command() = default;
    virtual void execute(UIContext& context, NodeEditorState& state) = 0;
};

} // namespace PGS::Gui

