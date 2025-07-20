#include "PGS/node_graph/nodes/rgb_node.h"

#include "PGS/node_graph/helpers.h"

PGS::NodeGraph::RGBNode::RGBNode(const NodeID id, std::string name)
    : Node(id, std::move(name))
{
    // Input
    registerInputPort({ "in_color", "", DataType::Color, sf::Color::White });

    // Output
    registerOutputPort({ "out_color", "Color", DataType::Color });
}

std::unordered_map<PGS::NodeGraph::PortID, PGS::NodeGraph::NodeData> PGS::NodeGraph::RGBNode::calculate(
    std::unordered_map<PortID, NodeData>& inputs, const sf::Vector2u& bufferSize) const
{
    auto color = getRequiredInput<std::shared_ptr<PixelBuffer>>(inputs, "in_color", bufferSize);

    return {{"out_color", color}};
}
