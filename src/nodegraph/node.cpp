#include "PGS/nodegraph/node.h"

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
    return m_inputPorts.at(id);
}

const PGS::NodeGraph::OutputPort& PGS::NodeGraph::Node::getOutputPort(const PortID& id) const
{
    return m_outputPorts.at(id);
}

const std::unordered_map<PGS::NodeGraph::PortID, PGS::NodeGraph::InputPort>& PGS::NodeGraph::Node::getInputPorts() const
{
    return m_inputPorts;
}

const std::unordered_map<PGS::NodeGraph::PortID, PGS::NodeGraph::OutputPort>& PGS::NodeGraph::Node::getOutputPorts() const
{
    return m_outputPorts;
}


// -- Setters --
void PGS::NodeGraph::Node::setName(std::string name)
{
    m_name = std::move(name);
}

// -- Protected Methods --
void PGS::NodeGraph::Node::registerInputPort(InputPort port)
{
    m_inputPorts[port.id] = std::move(port);
}

void PGS::NodeGraph::Node::registerOutputPort(OutputPort port)
{
    m_outputPorts[port.id] = std::move(port);
}