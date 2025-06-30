#pragma once

#include "PGS/nodegraph/types.h"

#include <SFML/System/Vector2.hpp>

#include <string>
#include <unordered_map>

namespace PGS::NodeGraph
{

class Node
{
private:
    const NodeID m_id;
    std::string m_name;

    std::unordered_map<PortID, InputPort> m_inputPorts;
    std::unordered_map<PortID, OutputPort> m_outputPorts;

public:
    // -- Constructors | Destructor --
    Node(NodeID id, std::string name);
    virtual ~Node() = default;

    // No-copyable
    Node(const Node&) = delete;
    Node& operator=(const Node&) = delete;

    // -- Getters --
    NodeID getID() const;
    const std::string& getName() const;

    const InputPort& getInputPort(const PortID& id) const;
    const OutputPort& getOutputPort(const PortID& id) const;
    const std::unordered_map<PortID, InputPort>& getInputPorts() const;
    const std::unordered_map<PortID, OutputPort>& getOutputPorts() const;

    // -- Setters --
    void setName(std::string name);

    template<typename T>
    void setInputPortValue(const PortID& id, T value);

    // -- Main Methods --
    virtual std::unordered_map<PortID, NodeData> calculate(
        std::unordered_map<PortID, NodeData>& inputs, const sf::Vector2u& bufferSize) const = 0;

protected:
    void registerInputPort(InputPort port);
    void registerOutputPort(OutputPort port);

    template<typename T>
    T getInputPortValue(const PortID& id) const;
};

} // namespace PGS::NodeGraph

template <typename T>
void PGS::NodeGraph::Node::setInputPortValue(const PortID& id, T value)
{
    if (const auto it = m_inputPorts.find(id); it != m_inputPorts.end())
    {
        it->second.value = value;
    }
}

template <typename T>
T PGS::NodeGraph::Node::getInputPortValue(const PortID& id) const
{
    const auto it = m_inputPorts.find(id);
    assert(it != m_inputPorts.end() && "Input port with this ID not found!");
    return std::get<T>(it->second.value);
}
