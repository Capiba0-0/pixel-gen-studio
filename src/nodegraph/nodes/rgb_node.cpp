#include "PGS/nodegraph/nodes/rgb_node.h"

PGS::NodeGraph::RGBNode::RGBNode(const NodeID id, std::string name)
    : Node(id, std::move(name))
{
    // Input
    registerInputPort({
        .id = "in_color",
        .name = "",
        .type = DataType::Color,
        .value = sf::Color::White
    });

    // Output
    registerOutputPort({
        .id = "out_color",
        .name = "Color",
        .type = DataType::Color
    });
}

std::unordered_map<PGS::NodeGraph::PortID, PGS::NodeGraph::NodeData> PGS::NodeGraph::RGBNode::calculate(
    std::unordered_map<PortID, NodeData>& inputs, const sf::Vector2u& bufferSize) const
{
    auto color = getRequiredInput<std::shared_ptr<PixelBuffer>>(inputs, "in_color", bufferSize);

    return {{"out_color", color}};
}
