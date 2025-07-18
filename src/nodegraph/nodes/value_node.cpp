#include "PGS/nodegraph/nodes/value_node.h"

PGS::NodeGraph::ValueNode::ValueNode(const NodeID id, std::string name)
    : Node(id, std::move(name))
{
    // Input
    registerInputPort({
        .id = "in_value",
        .name = "",
        .type = DataType::Number,
        .value = 0.0f
    });

    // Output
    registerOutputPort({
        .id = "out_value",
        .name = "Value",
        .type = DataType::Number
    });
}

std::unordered_map<PGS::NodeGraph::PortID, PGS::NodeGraph::NodeData> PGS::NodeGraph::ValueNode::calculate(
    std::unordered_map<PortID, NodeData>& inputs, const sf::Vector2u& bufferSize) const
{
    auto value = getRequiredInput<float>(inputs, "in_value", bufferSize);

    return {{"out_value", value}};
}
