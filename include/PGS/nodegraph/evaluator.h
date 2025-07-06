#pragma once

#include "PGS/nodegraph/types.h"

#include "PGS/nodegraph/node.h"
#include "PGS/nodegraph/nodes/texture_output_node.h"

#include <SFML/System/Vector2.hpp>

#include <utility>
#include <vector>
#include <memory>
#include <typeindex>
#include <unordered_map>
#include <map>
#include <functional>
#include <string>

namespace PGS::NodeGraph
{
// -- Declaration --
class Node;
class EvaluatorObserver;

class Evaluator
{
private:
    struct NodeFactoryInfo
    {
        std::string name;
        std::function<std::unique_ptr<Node>(NodeID, std::string)> factoryFunction;
    };

    std::unordered_map<std::type_index, NodeFactoryInfo> m_nodeFactories;

    NodeID m_nextNodeID = 1;

    std::unordered_map<NodeID, std::unique_ptr<Node>> m_nodes;
    std::map<NodeID, TextureOutputNode*> m_outputNodes;

    std::unordered_map<InputPortLocator, Connection> m_inputConnections;
    std::unordered_map<OutputPortLocator, std::vector<Connection>> m_outputConnections;

    std::unordered_map<NodeID, bool> m_dirtyFlags;
    std::unordered_map<NodeID, std::unordered_map<PortID, NodeData>> m_nodeCaches;

    std::vector<EvaluatorObserver*> m_observers;

    NodeID generateNextNodeID();

    void propagateDirtyFlag(NodeID nodeId);
    bool checkForCycle(NodeID sourceNode, NodeID targetNode);

    static NodeData convertValueToNodeData(const InputPortValue& value, const sf::Vector2u& bufferSize);

    void notifyNodeAdded(NodeID id, const Node& node) const;
    void notifyNodeRemoved(NodeID id) const;
    void notifyConnectionAdded(const Connection& connection) const;
    void notifyConnectionRemoved(const Connection& connection) const;
public:
    Evaluator();
    ~Evaluator() = default;

    template <typename T>
    void registerNode(std::string name);
    NodeID addNode(const std::type_index& typeIndex);
    void deleteNode(const NodeID& nodeId);

    template <typename T>
    void setNodeInputPortValue(const NodeID& nodeId, const PortID& id, T value);

    const std::unordered_map<NodeID, std::unique_ptr<Node>>& getNodes() const;

    void addConnection(const Connection& connection);
    void deleteConnection(const Connection& connection);

    const std::unordered_map<InputPortLocator, Connection>& getConnections() const;

    const std::unordered_map<std::type_index, NodeFactoryInfo>& getNodeFactories() const;

    void addObserver(EvaluatorObserver* observer);
    void removeObserver(EvaluatorObserver* observer);

    NodeData evaluate(NodeID nodeId, const PortID& portId, const sf::Vector2u& bufferSize);
    std::shared_ptr<PixelBuffer> evaluateFinalOutput(const sf::Vector2u& bufferSize);
};

} // namespace PGS::NodeGraph

template <typename T>
void PGS::NodeGraph::Evaluator::registerNode(std::string name)
{
    static_assert(std::is_base_of_v<Node, T>, "Type must be derived from PGS::NodeGraph::Node");

    const std::type_index typeIndex = typeid(T);

    auto& nodeFactoryInfo = m_nodeFactories[typeIndex];

    nodeFactoryInfo.name = std::move(name);

    nodeFactoryInfo.factoryFunction = [](NodeID id, std::string name)
    {
        return std::make_unique<T>(id, std::move(name));
    };
}

template <typename T>
void PGS::NodeGraph::Evaluator::setNodeInputPortValue(const NodeID& nodeId, const PortID& portId, T value)
{
    if (const auto nodeIt = m_nodes.find(nodeId); nodeIt != m_nodes.end())
    {
        nodeIt->second->setInputPortValue<T>(portId, value);
        propagateDirtyFlag(nodeId);
    }
}