#pragma once

#include "PGS/gui/widget.h"

#include "SFML/System/Vector2.hpp"

namespace PGS::Gui
{
// Declaration
class NodeEditorState;
class NodeEditorRenderer;
class NodeEditorInputHandler;

class NodeEditorWidget final : public Widget
{
public:
    NodeEditorWidget();
    ~NodeEditorWidget() override;

    void renderContent(UIContext& context) override;

    void initialize(NodeGraph::Evaluator& evaluator);

private:
    std::unique_ptr<NodeEditorInputHandler> m_inputHandler;
    std::unique_ptr<NodeEditorState> m_state;
    std::unique_ptr<NodeEditorRenderer> m_renderer;
    NodeGraph::Evaluator* m_evaluatorPtr = nullptr;

    sf::Vector2u m_lastMainWindowSize = {0, 0};
};

} // namespace PGS::Gui
