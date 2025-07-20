#pragma once

#include "PGS/node_graph/types.h"
#include "PGS/node_graph/node.h"
#include "PGS/node_graph/evaluator_observer.h"

#include <unordered_map>
#include <optional>

// Declaration
namespace PGS::NodeGraph
{
    class Evaluator;
}

namespace PGS::Gui
{
// Declaration
struct UIContext;

class NodeEditorState final : public NodeGraph::EvaluatorObserver
{
public:
    NodeEditorState() = default;

    int getIntPortID(const NodeGraph::OutputPortLocator& outputPortLocator) const;
    int getIntPortID(const NodeGraph::InputPortLocator& inputPortLocator) const;
    int getConnectionID(const NodeGraph::Connection& connection) const;

    std::optional<NodeGraph::Connection> getConnectionByID(int id) const;
    std::optional<NodeGraph::InputPortLocator> getInputPortLocatorByID(int id) const;
    std::optional<NodeGraph::OutputPortLocator> getOutputPortLocatorByID(int id) const;

    void syncWithEvaluator(const NodeGraph::Evaluator& evaluator);

    void onPortAdded(const NodeGraph::OutputPortLocator& outputPortLocator);
    void onPortAdded(const NodeGraph::InputPortLocator& inputPortLocator);

    // Override
    void onNodeAdded(NodeGraph::NodeID nodeId, const NodeGraph::Node& node) override;
    void onNodeRemoved(NodeGraph::NodeID nodeId) override;

    void onConnectionAdded(const NodeGraph::Connection& connection) override;
    void onConnectionRemoved(const NodeGraph::Connection& connection) override;

private:
    // TODO: Make Bimap instead of two std::unordered_map's
    std::unordered_map<int, NodeGraph::OutputPortLocator> m_intToOutputPortLocator;
    std::unordered_map<NodeGraph::OutputPortLocator, int> m_outputPortLocatorToInt;

    std::unordered_map<int, NodeGraph::InputPortLocator> m_intToInputPortLocator;
    std::unordered_map<NodeGraph::InputPortLocator, int> m_inputPortLocatorToInt;

    std::unordered_map<int, NodeGraph::Connection> m_intToConnection;
    std::unordered_map<NodeGraph::Connection, int> m_connectionToInt;

    int m_nextAttributeId = 1;
    int m_nextConnectionId = 1;

    size_t m_lastKnownNodeCount = 0;

    int generateNextAttributeID();
    int generateNextConnectionID();
};

} // namespace PGS::Gui