#include "PGS/nodegraph/evaluator.h"

// -- PGS Headers --
#include "PGS/nodegraph/types.h"
#include "PGS/core/buffers/pixel_buffer.h"
#include "PGS/nodegraph/evaluator_observer.h"

// -- Nodes --
#include "PGS/nodegraph/node.h"
//
#include "PGS/nodegraph/nodes/texture_output_node.h"
//
#include "PGS/nodegraph/nodes/checker_pattern_node.h"
#include "PGS/nodegraph/nodes/noise_texture_node.h"
#include "PGS/nodegraph/nodes/voronoi_texture_node.h"
//
#include "PGS/nodegraph/nodes/circle_pattern_node.h"
#include "PGS/nodegraph/nodes/rectangle_pattern_node.h"
//
#include "PGS/nodegraph/nodes/value_node.h"
#include "PGS/nodegraph/nodes/rgb_node.h"
//
#include "PGS/nodegraph/nodes/math_node.h"

// -- STL Headers --
#include <algorithm>
#include <cassert>

// -- Constructor --
PGS::NodeGraph::Evaluator::Evaluator()
{
    registerNode<TextureOutputNode>("Texture Output");

    registerNode<CheckerPatternNode>("Checker Texture");
    registerNode<NoiseTextureNode>("Noise Texture");
    registerNode<VoronoiTextureNode>("Voronoi Texture");

    registerNode<CirclePatternNode>("Circle");
    registerNode<RectanglePatternNode>("Rectangle");

    registerNode<ValueNode>("Value");
    registerNode<RGBNode>("RGB");

    registerNode<MathNode>("Math");

    addNode(typeid(TextureOutputNode));
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

    const auto outputPorts = nodeIt->second->getOutputPorts();
    for (const auto& outputPort : outputPorts)
    {
        OutputPortLocator outputPortLocator{nodeId, outputPort.id};

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

    return std::any_of(inputPorts.begin(), inputPorts.end(), [&](const auto& port)
    {
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
        if constexpr (std::is_same_v<T, ValueList>)
        {
            float index = argInput.first;
            return index;
        }

        assert(false && "Unhandled type in variant!");
    }, value);
}

void PGS::NodeGraph::Evaluator::notifyNodeAdded(const NodeID id, const Node& node) const
{
    for (const auto observer : m_observers)
    {
        observer->onNodeAdded(id, node);
    }
}

void PGS::NodeGraph::Evaluator::notifyNodeRemoved(const NodeID id) const
{
    for (const auto observer : m_observers)
    {
        observer->onNodeRemoved(id);
    }
}

void PGS::NodeGraph::Evaluator::notifyConnectionAdded(const Connection& connection) const
{
    for (const auto observer : m_observers)
    {
        observer->onConnectionAdded(connection);
    }
}

void PGS::NodeGraph::Evaluator::notifyConnectionRemoved(const Connection& connection) const
{
    for (const auto observer : m_observers)
    {
        observer->onConnectionRemoved(connection);
    }
}


// -- Public Methods --
PGS::NodeGraph::NodeID PGS::NodeGraph::Evaluator::addNode(const std::type_index& typeIndex)
{
    const auto factory = m_nodeFactories.find(typeIndex);
    if (factory == m_nodeFactories.end())
        return INVALID_NODE_ID;

    const NodeID nextId = generateNextNodeID();
    const std::string defaultName = factory->second.name;

    m_nodes[nextId] = factory->second.factoryFunction(nextId, defaultName);

    if (auto* outputNode = dynamic_cast<TextureOutputNode*>(m_nodes[nextId].get())) {
        m_outputNodes[nextId] = outputNode;
    }

    notifyNodeAdded(nextId, *m_nodes[nextId]);

    return nextId;
}

void PGS::NodeGraph::Evaluator::deleteNode(const NodeID& nodeId)
{
    const auto nodeIt = m_nodes.find(nodeId);
    if (nodeIt == m_nodes.end())
        return;

    auto inputPorts = nodeIt->second->getInputPorts();
    for (const auto& inputPort : inputPorts)
    {
        auto connectionIt = m_inputConnections.find({nodeId, inputPort.id});
        if (connectionIt == m_inputConnections.end())
            continue;

        deleteConnection(connectionIt->second);
    }
    auto outputPorts = nodeIt->second->getOutputPorts();
    for (const auto& outputPort : outputPorts)
    {
        auto connectionIt = m_outputConnections.find({nodeId, outputPort.id});
        if (connectionIt == m_outputConnections.end())
            continue;

        auto connectionToProcess = connectionIt->second; // Copying for security
        for (const auto& connection : connectionToProcess)
        {
            deleteConnection(connection);
        }
    }

    notifyNodeRemoved(nodeId);

    m_dirtyFlags.erase(nodeId);
    m_nodeCaches.erase(nodeId);
    m_nodes.erase(nodeIt);
    m_outputNodes.erase(nodeId);
}

const std::unordered_map<PGS::NodeGraph::NodeID, std::unique_ptr<PGS::NodeGraph::Node>>& PGS::NodeGraph::Evaluator::getNodes() const
{
    return m_nodes;
}


void PGS::NodeGraph::Evaluator::addConnection(const Connection& connection)
{
    // Validity check
    const auto sourceNode = m_nodes.find(connection.sourceNodeId);
    const auto targetNode = m_nodes.find(connection.targetNodeId);

    if (sourceNode == m_nodes.end() || targetNode == m_nodes.end())
        return;

    if (!sourceNode->second->isPort(connection.sourcePortId) ||
        !targetNode->second->isPort(connection.targetPortId))
        return;

    // Check for types
    const auto sourcePort = sourceNode->second->getOutputPort(connection.sourcePortId);
    const auto targetPort = targetNode->second->getInputPort(connection.targetPortId);

    if (!canConvert(sourcePort.type, targetPort.type))
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

    notifyConnectionAdded(connection);
}

void PGS::NodeGraph::Evaluator::deleteConnection(const Connection& connection)
{
    const InputPortLocator inputPortLocator{connection.targetNodeId, connection.targetPortId};
    const OutputPortLocator outputPortLocator{connection.sourceNodeId, connection.sourcePortId};

    const auto inputIt = m_inputConnections.find(inputPortLocator);
    if (inputIt == m_inputConnections.end())
        return;

    const auto outputIt = m_outputConnections.find(outputPortLocator);
    if (outputIt == m_outputConnections.end())
        return;

    m_inputConnections.erase(inputIt);

    auto& vec = outputIt->second;
    const auto newEnd = std::remove_if(vec.begin(), vec.end(),
    [&](const Connection& c) {
        return c.targetNodeId == connection.targetNodeId && c.targetPortId == connection.targetPortId;
    });
    vec.erase(newEnd, vec.end());

    notifyConnectionRemoved(connection);

    propagateDirtyFlag(connection.targetNodeId);
}

const std::unordered_map<PGS::NodeGraph::InputPortLocator, PGS::NodeGraph::Connection>& PGS::NodeGraph::Evaluator::getConnections() const
{
    return m_inputConnections;
}

const std::unordered_map<std::type_index, PGS::NodeGraph::Evaluator::NodeFactoryInfo>& PGS::NodeGraph::Evaluator::getNodeFactories() const
{
    return m_nodeFactories;
}


void PGS::NodeGraph::Evaluator::addObserver(EvaluatorObserver* observer)
{
    m_observers.push_back(observer);
}

void PGS::NodeGraph::Evaluator::removeObserver(EvaluatorObserver* observer)
{
    m_observers.erase(std::remove(m_observers.begin(), m_observers.end(), observer), m_observers.end());
}


PGS::NodeGraph::NodeData PGS::NodeGraph::Evaluator::evaluate(const NodeID nodeId, const PortID& portId, const sf::Vector2u& bufferSize)
{
    // Check for cache
    if (m_dirtyFlags.count(nodeId) && m_dirtyFlags.at(nodeId) == false)
    {
        if (auto const nodeCacheIt = m_nodeCaches.find(nodeId); nodeCacheIt != m_nodeCaches.end())
            if (const auto portCacheIt = nodeCacheIt->second.find(portId); portCacheIt != nodeCacheIt->second.end())
            {
                // Check for cache validity
                NodeData cachedData = portCacheIt->second;

                if (const auto pixelBuffer = std::get_if<std::shared_ptr<PixelBuffer>>(&cachedData))
                {
                    if ((*pixelBuffer)->getSize() == bufferSize)
                    {
                        return cachedData;
                    }
                }
                else if (const auto grayscaleBuffer = std::get_if<std::shared_ptr<GrayscaleBuffer>>(&cachedData))
                {
                    if ((*grayscaleBuffer)->getSize() == bufferSize)
                    {
                        return cachedData;
                    }
                }
                else if (const auto vectorFieldBuffer = std::get_if<std::shared_ptr<VectorFieldBuffer>>(&cachedData))
                {
                    if ((*vectorFieldBuffer)->getSize() == bufferSize)
                    {
                        return cachedData;
                    }
                }
                else // float, etc.
                {
                    return cachedData;
                }
            }
    }

    const auto nodeIt = m_nodes.find(nodeId);
    assert(nodeIt != m_nodes.end() && ("Failed to find node by id " + std::to_string(nodeId)).c_str());

    std::unordered_map<PortID, NodeData> inputs;

    const auto inputPorts = nodeIt->second->getInputPorts();
    for (const auto& inputPort : inputPorts)
    {
        auto inputIt = m_inputConnections.find({nodeId, inputPort.id});
        if (inputIt != m_inputConnections.end())
        {
            inputs[inputPort.id] = evaluate(inputIt->second.sourceNodeId, inputIt->second.sourcePortId, bufferSize);
        }
        else
        {
            const auto& sourceNode = m_nodes.at(nodeId);

            if (sourceNode->getInputPort(inputPort.id).value.has_value())
            {
                inputs[inputPort.id] = convertValueToNodeData(
                sourceNode->getInputPort(inputPort.id).value.value(),
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

std::shared_ptr<PGS::PixelBuffer> PGS::NodeGraph::Evaluator::evaluateFinalOutput(const sf::Vector2u& bufferSize)
{
    Connection const* targetConnection = nullptr;

    for (const auto& [nodeId, outputNodePtr] : m_outputNodes)
    {
        const PortID inputPortId = "in_color";
        InputPortLocator locator{nodeId, inputPortId};

        if (auto it = m_inputConnections.find(locator); it != m_inputConnections.end())
        {
            targetConnection = &it->second;
            break;
        }
    }

    if (!targetConnection)
        return nullptr;

    NodeData resultData = evaluate(
        targetConnection->sourceNodeId,
        targetConnection->sourcePortId,
        bufferSize
    );

    auto finalBufferOpt = getConvertedNodeData<std::shared_ptr<PixelBuffer>>(resultData, bufferSize,
        [&](){
            auto fallbackBuffer = std::make_shared<PixelBuffer>(bufferSize);
            fallbackBuffer->clear(sf::Color::Black);

            return fallbackBuffer;
        }
    );

    return finalBufferOpt.value();
}
