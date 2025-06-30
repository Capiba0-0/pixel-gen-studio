#include "PGS/nodegraph/evaluator.h"

#include "PGS/nodegraph/node.h"
#include "PGS/nodegraph/types.h"
#include "PGS/core/buffers/pixel_buffer.h"

#include <algorithm>
#include <cassert>

// -- Constructor --
PGS::NodeGraph::Evaluator::Evaluator()
{

}

// -- Private Methods --
PGS::NodeGraph::NodeID PGS::NodeGraph::Evaluator::generateNextNodeID()
{
    return m_nextNodeID++;
}

// Propagates the dirty flag recursively through the node graph.
// NOTE: This function is safe from infinite recursion because:
// 1. The node graph is guaranteed to be a Directed Acyclic Graph (DAG)
//    due to cycle checks in `addConnection`.
// 2. A check for an already-set dirty flag provides a hard stop,
//    preventing redundant traversal of any branch.
void PGS::NodeGraph::Evaluator::propagateDirtyFlag(const NodeID nodeId)
{
    auto dirtyFlagIt = m_dirtyFlags.find(nodeId);

    if (dirtyFlagIt != m_dirtyFlags.end() && dirtyFlagIt->second == true)
        return;

    m_dirtyFlags[nodeId] = true;

    const auto nodeIt = m_nodes.find(nodeId);
    if (nodeIt == m_nodes.end())
        return;

    auto outputPorts = nodeIt->second->getOutputPorts();
    for (const auto& [portId, port] : outputPorts)
    {
        OutputPortLocator outputPortLocator{nodeId, portId};

        auto connectionIt = m_outputConnections.find(outputPortLocator);
        if (connectionIt == m_outputConnections.end())
            continue;

        for (const auto& connection : connectionIt->second)
        {
            propagateDirtyFlag(connection.targetNodeId);
        }
    }
}

bool PGS::NodeGraph::Evaluator::checkForCycle(const NodeID sourceNode, const NodeID targetNode)
{
    if (sourceNode == targetNode)
        return true;

    const auto node = m_nodes.find(sourceNode);

    assert(node != m_nodes.end() && "Attempted to check a cycle for a non-existent node!");

    auto inputPorts = node->second->getInputPorts();

    return std::any_of(inputPorts.begin(), inputPorts.end(), [&](const auto& portPair)
    {
        const auto& port = portPair.second;

        const auto inputIt = m_inputConnections.find({sourceNode, port.id});
        if (inputIt == m_inputConnections.end())
            return false;

        return checkForCycle(inputIt->second.sourceNodeId, targetNode);
    });
}

PGS::NodeGraph::NodeData PGS::NodeGraph::Evaluator::convertValueToNodeData(const InputPortValue& value, const sf::Vector2u& bufferSize)
{
    return std::visit([&](auto&& argInput) -> NodeData
    {
        using T = std::decay_t<decltype(argInput)>;

        if constexpr (std::is_same_v<T, float>)
        {
            return argInput;
        }
        if constexpr (std::is_same_v<T, int>)
        {
            return static_cast<float>(argInput);
        }
        if constexpr (std::is_same_v<T, bool>)
        {
            return argInput ? 1.0f : 0.0f;
        }
        if constexpr (std::is_same_v<T, sf::Color>)
        {
            auto buffer {std::make_shared<PixelBuffer>(bufferSize)};
            buffer->clear(argInput);
            return buffer;
        }
        assert(false && "Unhandled type in variant!");
    }, value);
}


// -- Public Methods --
PGS::NodeGraph::NodeID PGS::NodeGraph::Evaluator::addNode(const std::type_index& typeIndex)
{
    const auto factory = m_nodeFactories.find(typeIndex);
    if (factory == m_nodeFactories.end())
        return INVALID_NODE_ID;

    const NodeID nextId = generateNextNodeID();
    const std::string defaultName = "Node #" + std::to_string(nextId);

    m_nodes[nextId] = factory->second(nextId, defaultName);

    return nextId;
}

void PGS::NodeGraph::Evaluator::deleteNode(const NodeID& nodeId)
{
    const auto node = m_nodes.find(nodeId);
    if (node == m_nodes.end())
        return;

    auto inputPorts = node->second->getInputPorts();
    for (const auto& [portId, port] : inputPorts)
    {
        auto connectionIt = m_inputConnections.find({nodeId, portId});
        if (connectionIt == m_inputConnections.end())
            continue;

        deleteConnection(connectionIt->second);
    }
    auto outputPorts = node->second->getOutputPorts();
    for (const auto& [portId, port] : outputPorts)
    {
        auto connectionIt = m_outputConnections.find({nodeId, portId});
        if (connectionIt == m_outputConnections.end())
            continue;

        auto connectionToProcess = connectionIt->second; // Copying for security
        for (const auto& connection : connectionToProcess)
        {
            deleteConnection(connection);
        }
    }

    m_dirtyFlags.erase(nodeId);
    m_nodeCaches.erase(nodeId);
    m_nodes.erase(node);
}

void PGS::NodeGraph::Evaluator::addConnection(const Connection& connection)
{
    // Validity check
    const auto sourceNode = m_nodes.find(connection.sourceNodeId);
    const auto targetNode = m_nodes.find(connection.targetNodeId);

    if (sourceNode == m_nodes.end() || targetNode == m_nodes.end())
        return;

    const auto& sourcePorts = sourceNode->second->getOutputPorts();
    const auto& targetPorts = targetNode->second->getInputPorts();

    if (sourcePorts.find(connection.sourcePortId) == sourcePorts.end() ||
        targetPorts.find(connection.targetPortId) == targetPorts.end())
        return;

    // Check for types
    const auto sourcePortType = sourceNode->second->getOutputPort(connection.sourcePortId).type;
    const auto targetPortType = targetNode->second->getInputPort(connection.targetPortId).type;

    if (sourcePortType != targetPortType)
        return;

    // Cycle check
    if (checkForCycle(connection.sourceNodeId, connection.targetNodeId))
        return;

    // Add connections
    const InputPortLocator inputPortLocator{connection.targetNodeId, connection.targetPortId};
    const OutputPortLocator outputPortLocator{connection.sourceNodeId, connection.sourcePortId};
    m_inputConnections[inputPortLocator] = connection;
    m_outputConnections[outputPortLocator].push_back(connection);

    // Propagate dirty
    propagateDirtyFlag(connection.targetNodeId);
}

void PGS::NodeGraph::Evaluator::deleteConnection(const Connection& connection)
{
    const InputPortLocator inputPortLocator{connection.targetNodeId, connection.targetPortId};
    const OutputPortLocator outputPortLocator{connection.sourceNodeId, connection.sourcePortId};

    const auto inputIt = m_inputConnections.find(inputPortLocator);
    if (inputIt == m_inputConnections.end())
        return;

    m_inputConnections.erase(inputIt);


    const auto outputIt = m_outputConnections.find(outputPortLocator);
    if (outputIt == m_outputConnections.end())
        return;

    auto& vec = outputIt->second;
    const auto newEnd = std::remove_if(vec.begin(), vec.end(),
    [&](const Connection& c) {
        return c.targetNodeId == connection.targetNodeId && c.targetPortId == connection.targetPortId;
    });
    vec.erase(newEnd, vec.end());

    propagateDirtyFlag(connection.targetNodeId);
}

PGS::NodeGraph::NodeData PGS::NodeGraph::Evaluator::evaluate(const NodeID nodeId, const PortID& portId, const sf::Vector2u& bufferSize)
{
    if (m_dirtyFlags.count(nodeId) && m_dirtyFlags.at(nodeId) == false)
    {
        if (auto const nodeCacheIt = m_nodeCaches.find(nodeId); nodeCacheIt != m_nodeCaches.end())
            if (const auto portCacheIt = nodeCacheIt->second.find(portId); portCacheIt != nodeCacheIt->second.end())
                return portCacheIt->second;
    }

    const auto nodeIt = m_nodes.find(nodeId);
    assert(nodeIt != m_nodes.end() && ("Failed to find node by id " + std::to_string(nodeId)).c_str());

    std::unordered_map<PortID, NodeData> inputs;

    auto inputPorts = nodeIt->second->getInputPorts();
    for (const auto& [inputPortId, port] : inputPorts)
    {
        auto inputIt = m_inputConnections.find({nodeId, inputPortId});
        if (inputIt != m_inputConnections.end())
        {
            inputs[inputPortId] = evaluate(inputIt->second.sourceNodeId, inputIt->second.sourcePortId, bufferSize);
        }
        else
        {
            const auto& sourceNode = m_nodes.at(nodeId);

            if (sourceNode->getInputPort(inputPortId).value.has_value())
            {
                inputs[inputPortId] = convertValueToNodeData(
                sourceNode->getInputPort(inputPortId).value.value(),
                bufferSize);
            }
        }
    }

    auto results = nodeIt->second->calculate(inputs, bufferSize);

    m_nodeCaches[nodeId] = results;
    m_dirtyFlags[nodeId] = false;

    if (results.count(portId))
    {
        return results.at(portId);
    }

    // Handling the case when a value is missing in the "results" for some reason
    auto buffer{std::make_shared<PixelBuffer>(bufferSize)};
    buffer->clear();

    return buffer;
}
