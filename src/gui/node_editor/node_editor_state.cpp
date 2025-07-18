#include "PGS/gui/node_editor/node_editor_state.h"

#include "PGS/gui/ui_context.h"
#include "PGS/nodegraph/node.h"

#include <optional>

int PGS::Gui::NodeEditorState::generateNextAttributeID()
{
    return m_nextAttributeId++;
}

int PGS::Gui::NodeEditorState::generateNextConnectionID()
{
    return m_nextConnectionId++;
}


int PGS::Gui::NodeEditorState::getIntPortID(const NodeGraph::OutputPortLocator& outputPortLocator) const
{
    auto it = m_outputPortLocatorToInt.find(outputPortLocator);
    if (it != m_outputPortLocatorToInt.end())
    {
        return it->second;
    }

    assert(false && "An out-of-sync occurred inside NodeEditorState");
}


int PGS::Gui::NodeEditorState::getIntPortID(const NodeGraph::InputPortLocator& inputPortLocator) const
{
    auto it = m_inputPortLocatorToInt.find(inputPortLocator);
    if (it != m_inputPortLocatorToInt.end())
    {
        return it->second;
    }

    assert(false && "An out-of-sync occurred inside NodeEditorState");
}

int PGS::Gui::NodeEditorState::getConnectionID(const NodeGraph::Connection& connection) const
{
    auto it = m_connectionToInt.find(connection);
    if (it != m_connectionToInt.end())
    {
        return it->second;
    }

    assert(false && "An out-of-sync occurred inside NodeEditorState");
}

std::optional<PGS::NodeGraph::Connection> PGS::Gui::NodeEditorState::getConnectionByID(const int id) const
{
    if (const auto it = m_intToConnection.find(id); it != m_intToConnection.end())
        return it->second;
    return std::nullopt;
}

std::optional<PGS::NodeGraph::InputPortLocator> PGS::Gui::NodeEditorState::getInputPortLocatorByID(int id) const
{
    if (const auto it = m_intToInputPortLocator.find(id); it != m_intToInputPortLocator.end())
        return it->second;
    return std::nullopt;
}

std::optional<PGS::NodeGraph::OutputPortLocator> PGS::Gui::NodeEditorState::getOutputPortLocatorByID(int id) const
{
    if (const auto it = m_intToOutputPortLocator.find(id); it != m_intToOutputPortLocator.end())
        return it->second;
    return std::nullopt;
}

void PGS::Gui::NodeEditorState::syncWithEvaluator(const NodeGraph::Evaluator& evaluator)
{
    m_intToOutputPortLocator.clear();
    m_outputPortLocatorToInt.clear();
    m_intToInputPortLocator.clear();
    m_inputPortLocatorToInt.clear();
    m_intToConnection.clear();
    m_connectionToInt.clear();

    for (const auto& [nodeId, nodePtr] : evaluator.getNodes())
    {
        onNodeAdded(nodeId, *nodePtr);
    }

    for (const auto& [portLoc, connection] : evaluator.getConnections())
    {
        onConnectionAdded(connection);
    }

    m_lastKnownNodeCount = evaluator.getNodes().size();
}


void PGS::Gui::NodeEditorState::onPortAdded(const NodeGraph::InputPortLocator& inputPortLocator)
{
    if (m_inputPortLocatorToInt.count(inputPortLocator) == 0)
    {
        const int newId = generateNextAttributeID();
        m_inputPortLocatorToInt[inputPortLocator] = newId;
        m_intToInputPortLocator[newId] = inputPortLocator;
    }
}

void PGS::Gui::NodeEditorState::onPortAdded(const NodeGraph::OutputPortLocator& outputPortLocator)
{
    if (m_outputPortLocatorToInt.count(outputPortLocator) == 0)
    {
        const int newId = generateNextAttributeID();
        m_outputPortLocatorToInt[outputPortLocator] = newId;
        m_intToOutputPortLocator[newId] = outputPortLocator;
    }
}


void PGS::Gui::NodeEditorState::onNodeAdded(const NodeGraph::NodeID nodeId, const NodeGraph::Node& node)
{
    for (const auto& outputPort : node.getOutputPorts())
    {
        onPortAdded(NodeGraph::OutputPortLocator{nodeId, outputPort.id});
    }

    for (const auto& inputPort : node.getInputPorts())
    {
        onPortAdded(NodeGraph::InputPortLocator{nodeId, inputPort.id});
    }
}

void PGS::Gui::NodeEditorState::onNodeRemoved(NodeGraph::NodeID nodeId)
{
    auto removePorts = [&](auto& forwardMap, auto& backwardMap)
    {
        for (auto it = forwardMap.begin(); it != forwardMap.end(); )
        {
            if (it->first.node == nodeId)
            {
                backwardMap.erase(it->second);
                it = forwardMap.erase(it);
            }
            else
                ++it;
        }
    };

    removePorts(m_outputPortLocatorToInt, m_intToOutputPortLocator);
    removePorts(m_inputPortLocatorToInt,m_intToInputPortLocator);

    for (auto it = m_connectionToInt.begin(); it != m_connectionToInt.end();)
    {
        if (it->first.sourceNodeId == nodeId || it->first.targetNodeId == nodeId)
        {
            m_intToConnection.erase(it->second);
            it = m_connectionToInt.erase(it);
        }
        else
            ++it;
    }
}


void PGS::Gui::NodeEditorState::onConnectionAdded(const NodeGraph::Connection& connection)
{
    if (m_connectionToInt.count(connection) == 0)
    {
        const int newId = generateNextConnectionID();
        m_connectionToInt[connection] = newId;
        m_intToConnection[newId] = connection;
    }
}

void PGS::Gui::NodeEditorState::onConnectionRemoved(const NodeGraph::Connection& connection)
{
    m_intToConnection.erase(getConnectionID(connection));
    m_connectionToInt.erase(connection);
}