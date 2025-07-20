#pragma once

#include "PGS/node_graph/types.h"

#include <SFML/System/Vector2.hpp>

#include <string>
#include <vector>
#include <unordered_map>

namespace PGS::NodeGraph
{

class Node
{
private:
    const NodeID m_id;
    std::string m_name;

    // TODO: It may be better to keep std::vector to maintain order
    std::vector<InputPort> m_inputPorts;
    std::vector<OutputPort> m_outputPorts;

public:
    // -- Constructors | Destructor --
    Node(NodeID id, std::string name);
    virtual ~Node() = default;

    // No-copyable
    Node(const Node&) = delete;
    Node& operator=(const Node&) = delete;

    // -- Getters --
    [[nodiscard]] NodeID getID() const;
    [[nodiscard]] const std::string& getName() const;

    [[nodiscard]] const InputPort& getInputPort(const PortID& id) const;
    [[nodiscard]] const OutputPort& getOutputPort(const PortID& id) const;
    [[nodiscard]] const std::vector<InputPort>& getInputPorts() const;
    [[nodiscard]] const std::vector<OutputPort>& getOutputPorts() const;

    [[nodiscard]] bool isPort(const PortID& id) const;

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
    for (auto& inputPort : m_inputPorts)
    {
        if (inputPort.id == id)
            inputPort.value = value;
    }
}

template <typename T>
T PGS::NodeGraph::Node::getInputPortValue(const PortID& id) const
{
    return std::get<T>(getInputPort(id).value);
}
