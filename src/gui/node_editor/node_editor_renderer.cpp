#include "PGS/gui/node_editor/node_editor_renderer.h"

#include "PGS/gui/ui_helpers.h"
#include "PGS/nodegraph/node.h"
#include "PGS/gui/node_editor/node_editor_state.h"
#include "PGS/gui/node_editor/input/commands.h"
#include "PGS/gui/node_editor/input/node_editor_input_handler.h"

#include "imnodes.h"

void PGS::Gui::NodeEditorRenderer::renderNodes(const UIContext& context, const NodeEditorState& state)
{
    for (const auto& [nodeId, node] : context.evaluator.getNodes())
    {
        ImNodes::BeginNode(static_cast<int>(nodeId));

        ImNodes::BeginNodeTitleBar();
        ImGui::TextUnformatted(node->getName().c_str());
        ImNodes::EndNodeTitleBar();

        constexpr float NODE_SIZE_X = 140;

        ImGui::Dummy({NODE_SIZE_X, 0});

        for (const auto& [outputPortId, outputPort] : node->getOutputPorts())
        {
            ImNodes::BeginOutputAttribute(state.getIntPortID(NodeGraph::OutputPortLocator{nodeId, outputPortId}));

            const float textWidth = ImGui::CalcTextSize(outputPort.name.c_str()).x;
            ImGui::SetCursorPosX(ImGui::GetCursorPosX() + NODE_SIZE_X - textWidth);

            ImGui::Text(outputPort.name.c_str());

            ImNodes::EndOutputAttribute();
        }
        for (auto& [inputPortId, inputPort] : node->getInputPorts())
        {
            ImNodes::BeginInputAttribute(state.getIntPortID(NodeGraph::InputPortLocator{nodeId, inputPortId}));

            if (inputPort.value.has_value())
            {
                auto it = context.evaluator.getConnections().find(NodeGraph::InputPortLocator{nodeId, inputPortId});
                if (it == context.evaluator.getConnections().end())
                {
                    std::visit([&](auto&& arg)
                    {
                        using T = std::decay_t<decltype(arg)>;

                        T value = arg;

                        if constexpr (std::is_same_v<T, float>)
                        {
                            ImGui::SetNextItemWidth(NODE_SIZE_X - ImGui::CalcTextSize(inputPort.name.c_str()).x);
                            if (ImGui::DragFloat(inputPort.name.c_str(), &value, 0.5,
                                inputPort.metadata.minValue, inputPort.metadata.maxValue))
                                context.evaluator.setNodeInputPortValue(nodeId, inputPortId, value);
                        }

                        else if constexpr (std::is_same_v<T, int>)
                        {
                            ImGui::SetNextItemWidth(NODE_SIZE_X - ImGui::CalcTextSize(inputPort.name.c_str()).x);
                            if (ImGui::DragInt(inputPort.name.c_str(), &value, 0.2,
                                inputPort.metadata.minValue, inputPort.metadata.maxValue))
                                context.evaluator.setNodeInputPortValue(nodeId, inputPortId, value);
                        }

                        else if constexpr (std::is_same_v<T, bool>)
                        {
                            if (ImGui::Checkbox(inputPort.name.c_str(), &value))
                                context.evaluator.setNodeInputPortValue(nodeId, inputPortId, value);
                        }

                        else if constexpr (std::is_same_v<T, sf::Color>)
                        {
                            float color[4] = {
                                value.r / 255.f,
                                value.g / 255.f,
                                value.b / 255.f,
                                value.a / 255.f
                            };

                            if (ImGui::ColorEdit4(inputPort.name.c_str(), color, ImGuiColorEditFlags_NoInputs)) {
                                value.r = static_cast<std::uint8_t>(color[0] * 255.f);
                                value.g = static_cast<std::uint8_t>(color[1] * 255.f);
                                value.b = static_cast<std::uint8_t>(color[2] * 255.f);
                                value.a = static_cast<std::uint8_t>(color[3] * 255.f);

                                context.evaluator.setNodeInputPortValue(nodeId, inputPortId, value);
                            }
                        }

                    }, *inputPort.value);
                }
            }
            else
            {
                ImGui::Text(inputPort.name.c_str());
            }
            ImNodes::EndInputAttribute();
        }

        ImNodes::EndNode();
    }
}

void PGS::Gui::NodeEditorRenderer::renderLinks(const UIContext& context, const NodeEditorState& state)
{
    for (const auto& [portLocator, connection] : context.evaluator.getConnections())
    {
        const int sourceAttrId = state.getIntPortID(NodeGraph::OutputPortLocator{connection.sourceNodeId, connection.sourcePortId});
        const int targetAttrId = state.getIntPortID(NodeGraph::InputPortLocator{connection.targetNodeId, connection.targetPortId});
        ImNodes::Link(state.getConnectionID(connection), sourceAttrId, targetAttrId);
    }
}

void PGS::Gui::NodeEditorRenderer::renderPopups(const UIContext& context, const NodeEditorState& state, std::vector<std::unique_ptr<Command>>& commands)
{
    ImGui::SetNextWindowSize(ImVec2{150, 0});
    if (ImGui::BeginPopup("Node Creation Menu", ImGuiWindowFlags_Popup))
    {
        ImGui::Text("Add Node");
        ImGui::Separator();

        nodeCreationMenu(context, commands);

        ImGui::EndPopup();
    }

    ImGui::SetNextWindowSize(ImVec2{150, 0});
    if (ImGui::BeginPopup("Node Editor Menu", ImGuiWindowFlags_Popup))
    {
        ImGui::Text("Node Editor");
        ImGui::Separator();

        if (ImGui::BeginMenu("Add Node"))
        {
            ImGui::Dummy(ImVec2{140, 0});
            nodeCreationMenu(context, commands);

            ImGui::EndMenu();
        }

        ImGui::EndPopup();
    }
}


// DRY Helpers
void PGS::Gui::NodeEditorRenderer::nodeCreationMenu(const UIContext& context, std::vector<std::unique_ptr<Command>>& commands)
{
    const auto& nodeFactories = context.evaluator.getNodeFactories();

    for (const auto& [typeIndex, factoryInfo] : nodeFactories)
    {
        if (ImGui::MenuItem(factoryInfo.name.c_str()))
        {
            commands.push_back(std::make_unique<AddNodeCommand>(typeIndex, ImGui::GetMousePos()));
            ImGui::CloseCurrentPopup();
        }
    }
}
