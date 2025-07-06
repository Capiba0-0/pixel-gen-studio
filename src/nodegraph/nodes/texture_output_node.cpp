#include "PGS/nodegraph/nodes/texture_output_node.h"

PGS::NodeGraph::TextureOutputNode::TextureOutputNode(const NodeID id, std::string name)
    : Node(id, std::move(name))
{
    // Input
    registerInputPort({
        .id = "in_color",
        .name = "Color",
        .type = DataType::Color
    });
}

std::unordered_map<PGS::NodeGraph::PortID, PGS::NodeGraph::NodeData> PGS::NodeGraph::TextureOutputNode::calculate(std::unordered_map<PortID, NodeData>& inputs, const sf::Vector2u& bufferSize) const
{
    assert(false && "Texture Output node can't calculate");
}
