#include "PGS/nodegraph/node.h"

#include <any>
#include <utility>

// -- Constructors | Destructor --
PGS::NodeGraph::Node::Node(const NodeID id, std::string name)
    : m_id(id)
    , m_name(std::move(name))
{}

// -- Getters --
PGS::NodeGraph::NodeID PGS::NodeGraph::Node::getID() const
{
    return m_id;
}

const std::string& PGS::NodeGraph::Node::getName() const
{
    return m_name;
}

const PGS::NodeGraph::InputPort& PGS::NodeGraph::Node::getInputPort(const PortID& id) const
{
    for (const auto& inputPort : m_inputPorts)
    {
        if (inputPort.id == id)
            return inputPort;
    }

    assert(false && "The entered input port ID is not valid");
}

const PGS::NodeGraph::OutputPort& PGS::NodeGraph::Node::getOutputPort(const PortID& id) const
{
    for (const auto& outputPort : m_outputPorts)
    {
        if (outputPort.id == id)
            return outputPort;
    }

    assert(false && "The entered output port ID is not valid");
}

const std::vector<PGS::NodeGraph::InputPort>& PGS::NodeGraph::Node::getInputPorts() const
{
    return m_inputPorts;
}

const std::vector<PGS::NodeGraph::OutputPort>& PGS::NodeGraph::Node::getOutputPorts() const
{
    return m_outputPorts;
}

bool PGS::NodeGraph::Node::isPort(const PortID& id) const
{
    return
        std::any_of(m_outputPorts.begin(), m_outputPorts.end(),
                   [&](const auto& outputPort) { return outputPort.id == id; }) ||

        std::any_of(m_inputPorts.begin(), m_inputPorts.end(),
                   [&](const auto& inputPort) { return inputPort.id == id; });
}


// -- Setters --
void PGS::NodeGraph::Node::setName(std::string name)
{
    m_name = std::move(name);
}

// -- Protected Methods --
void PGS::NodeGraph::Node::registerInputPort(InputPort port)
{
    m_inputPorts.push_back(std::move(port));
}

void PGS::NodeGraph::Node::registerOutputPort(OutputPort port)
{
    m_outputPorts.push_back(std::move(port));
}