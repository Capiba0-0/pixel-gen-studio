#include "PGS/gui/node_editor/node_editor_widget.h"

#include "PGS/gui/node_editor/node_editor_state.h"
#include "PGS/gui/node_editor/input/node_editor_input_handler.h"
#include "PGS/gui/node_editor/node_editor_renderer.h"

#include "imnodes.h"
#include "imgui.h"

namespace PGS::NodeGraph
{
    class TextureOutputNode;
}

PGS::Gui::NodeEditorWidget::NodeEditorWidget()
    : m_inputHandler(std::make_unique<NodeEditorInputHandler>())
    , m_state(std::make_unique<NodeEditorState>())
    , m_renderer(std::make_unique<NodeEditorRenderer>())
{}

PGS::Gui::NodeEditorWidget::~NodeEditorWidget()
{
    if (m_evaluatorPtr) {
        m_evaluatorPtr->removeObserver(m_state.get());
    }
}

void PGS::Gui::NodeEditorWidget::renderContent(UIContext& context)
{
    const ImGuiViewport* viewport = ImGui::GetMainViewport();

    const float desiredWidth = viewport->WorkSize.x / 3.0f;
    const auto size = ImVec2{desiredWidth, viewport->WorkSize.y};
    const auto pos = ImVec2{viewport->WorkPos.x + viewport->WorkSize.x - desiredWidth, viewport->WorkPos.y};

    ImGui::SetNextWindowPos(pos, ImGuiCond_Always);
    ImGui::SetNextWindowSize(size, ImGuiCond_Always);

    // Node Editor Window
    ImGui::Begin("Node Editor", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize);

    ImNodes::BeginNodeEditor();

    m_renderer->renderNodes(context, *m_state);
    m_renderer->renderLinks(context, *m_state);

    ImNodes::MiniMap(0.2, ImNodesMiniMapLocation_BottomRight);
    ImNodes::EndNodeEditor();

    std::vector<std::unique_ptr<Command>> frameCommands;

    // TODO: Rework this method in accordance with the Dependency Inversion principle
    m_inputHandler->processInput(frameCommands);
    m_renderer->renderPopups(context, *m_state, frameCommands);

    for (const auto& command : frameCommands)
    {
        command->execute(context, *m_state);
    }

    ImGui::End();
}

void PGS::Gui::NodeEditorWidget::initialize(NodeGraph::Evaluator& evaluator)
{
    m_evaluatorPtr = &evaluator;
    m_evaluatorPtr->addObserver(m_state.get());
    m_state->syncWithEvaluator(*m_evaluatorPtr);
}
