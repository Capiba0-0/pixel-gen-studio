#pragma once

#include "PGS/nodegraph/types.h"

#include <vector>
#include <memory>
#include <typeindex>
#include <unordered_map>
#include <functional>
#include <string>
#include <SFML/System/Vector2.hpp>

namespace PGS::NodeGraph
{
// -- Declaration --
class Node;

class Evaluator
{
private:
    using NodeFactory = std::function<std::unique_ptr<Node>(NodeID, std::string)>;
    std::unordered_map<std::type_index, NodeFactory> m_nodeFactories;

    NodeID m_nextNodeID = 1;

    std::unordered_map<NodeID, std::unique_ptr<Node>> m_nodes;

    std::unordered_map<InputPortLocator, Connection> m_inputConnections;
    std::unordered_map<OutputPortLocator, std::vector<Connection>> m_outputConnections;

    std::unordered_map<NodeID, bool> m_dirtyFlags;
    std::unordered_map<NodeID, std::unordered_map<PortID, NodeData>> m_nodeCaches;

    NodeID generateNextNodeID();

    void propagateDirtyFlag(NodeID nodeId);
    bool checkForCycle(NodeID sourceNode, NodeID targetNode);

    static NodeData convertValueToNodeData(const InputPortValue& value, const sf::Vector2u& bufferSize) ;

public:
    Evaluator();
    ~Evaluator() = default;

    template <typename T>
    void registerNode();
    NodeID addNode(const std::type_index& typeIndex);
    void deleteNode(const NodeID& nodeId);

    void addConnection(const Connection& connection);
    void deleteConnection(const Connection& connection);

    NodeData evaluate(NodeID nodeId, const PortID& portId, const sf::Vector2u& bufferSize);
};

} // namespace PGS::NodeGraph

template <typename T>
void PGS::NodeGraph::Evaluator::registerNode()
{
    static_assert(std::is_base_of_v<Node, T>, "Type must be derived from PGS::NodeGraph::Node");

    const std::type_index typeIndex = typeid(T);

    m_nodeFactories[typeIndex] = [](NodeID id, std::string name)
    {
        return std::make_unique<T>(id, std::move(name));
    };
}