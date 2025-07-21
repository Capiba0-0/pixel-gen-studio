#include "PGS/gui/node_editor/input/commands.h"

#include "PGS/gui/ui_context.h"
#include "PGS/gui/node_editor/node_editor_state.h"

#include "imnodes.h"

#include <vector>

void PGS::Gui::DeleteSelectedCommand::execute(UIContext& context, NodeEditorState& state)
{
    int numSelectedLinks = ImNodes::NumSelectedLinks();
    if (numSelectedLinks > 0)
    {
        std::vector<int> selectedLinks(numSelectedLinks);

        ImNodes::GetSelectedLinks(selectedLinks.data());

        for (const int linkId : selectedLinks)
        {
            if (auto connectionOpt = state.getConnectionByID(linkId); connectionOpt) {
                context.evaluator.deleteConnection(*connectionOpt);
            }
        }
    }

    int numSelectedNodes = ImNodes::NumSelectedNodes();
    if (numSelectedNodes > 0)
    {
        std::vector<int> selectedNodeIds(numSelectedNodes);

        ImNodes::GetSelectedNodes(selectedNodeIds.data());

        for (const int nodeId : selectedNodeIds)
        {
            context.evaluator.deleteNode(static_cast<NodeGraph::NodeID>(nodeId));
        }
    }

    ImNodes::ClearLinkSelection();
    ImNodes::ClearNodeSelection();
}

void PGS::Gui::AddNodeCommand::execute(UIContext& context, NodeEditorState& state)
{
    NodeGraph::NodeID newNodeId = context.evaluator.addNode(m_type);

    if (newNodeId != NodeGraph::INVALID_NODE_ID)
    {
        ImNodes::SetNodeScreenSpacePos(static_cast<int>(newNodeId), m_position);
    }
}

void PGS::Gui::CreateLinkCommand::execute(UIContext& context, NodeEditorState& state)
{
    std::optional<NodeGraph::OutputPortLocator> sourceLocOpt = state.getOutputPortLocatorByID(m_startAttrId);
    if (!sourceLocOpt)
        sourceLocOpt = state.getOutputPortLocatorByID(m_endAttrId);

    std::optional<NodeGraph::InputPortLocator> targetLocOpt = state.getInputPortLocatorByID(m_startAttrId);
    if (!targetLocOpt)
        targetLocOpt = state.getInputPortLocatorByID(m_endAttrId);

    if (sourceLocOpt && targetLocOpt)
    {
        auto& sourceLoc = *sourceLocOpt;
        auto& targetLoc = *targetLocOpt;

        if (auto it = context.evaluator.getConnections().find(targetLoc);
            it != context.evaluator.getConnections().end())
        {
            context.evaluator.deleteConnection(it->second);
        }

        const NodeGraph::Connection newConnection{
            sourceLoc.node, sourceLoc.port,
            targetLoc.node, targetLoc.port
        };

        context.evaluator.addConnection(newConnection);
    }
}

void PGS::Gui::DestroyLinkCommand::execute(UIContext& context, NodeEditorState& state)
{
    if (const auto connectionOpt = state.getConnectionByID(m_linkId); connectionOpt) {
        context.evaluator.deleteConnection(*connectionOpt);
    }
}


void PGS::Gui::DropLinkCommand::execute(UIContext& context, NodeEditorState& state)
{
    if (state.getOutputPortLocatorByID(m_startAttrId).has_value() || state.getInputPortLocatorByID(m_startAttrId))
    {
        if (auto sourceLocOpt = state.getInputPortLocatorByID(m_startAttrId))
        {
            for (const auto& [portLoc, connection] : context.evaluator.getConnections())
            {
                if (connection.targetNodeId == sourceLocOpt->node && connection.targetPortId == sourceLocOpt->port)
                {
                    context.evaluator.deleteConnection(connection);
                    break;
                }
            }
        }
    }
}

