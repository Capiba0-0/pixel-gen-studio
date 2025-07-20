#include "PGS/node_graph/nodes/texture_output_node.h"

#include <stdexcept>

PGS::NodeGraph::TextureOutputNode::TextureOutputNode(const NodeID id, std::string name)
    : Node(id, std::move(name))
{
    // Input
    registerInputPort({ "in_color", "Color", DataType::Color });
}

std::unordered_map<PGS::NodeGraph::PortID, PGS::NodeGraph::NodeData> PGS::NodeGraph::TextureOutputNode::calculate(std::unordered_map<PortID, NodeData>& inputs, const sf::Vector2u& bufferSize) const
{
    throw std::runtime_error("Texture Output node can't calculate");
}
