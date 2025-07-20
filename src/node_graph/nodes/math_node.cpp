#include "PGS/node_graph/nodes/math_node.h"

#include "PGS/node_graph/helpers.h"

#include "cmath"

PGS::NodeGraph::MathNode::MathNode(const NodeID id, std::string name)
    : Node(id, std::move(name))
{
    // Input
    registerInputPort({
        .id = "in_action",
        .name = "",
        .type = DataType::Number,
        .value = ValueList{0,
            {"Func: Add", "Func: Subtract", "Func: Multiply",
                   "Func: Divide", "Func: Power", "Func: Logarithm",
                   "Comparison: Minimum", "Comparison: Maximum"}}
    });

    registerInputPort({
        .id = "in_value1",
        .name = "Value1",
        .type = DataType::Number,
        .value = 0.0f
    });

    registerInputPort({
        .id = "in_value2",
        .name = "Value2",
        .type = DataType::Number,
        .value = 0.0f
    });

    // Output
    registerOutputPort({
        .id = "out_result",
        .name = "Result",
        .type = DataType::Number
    });
}

std::unordered_map<PGS::NodeGraph::PortID, PGS::NodeGraph::NodeData> PGS::NodeGraph::MathNode::calculate(
    std::unordered_map<PortID, NodeData>& inputs, const sf::Vector2u& bufferSize) const
{
    const int action = static_cast<int>(getRequiredInput<float>(inputs, "in_action", bufferSize));

    const auto value1 = getRequiredInput<float>(inputs, "in_value1", bufferSize);
    const auto value2 = getRequiredInput<float>(inputs, "in_value2", bufferSize);

    float result = 0.0f;

    switch (action)
    {
    case 0:
        result = value1 + value2;
        break;
    case 1:
        result = value1 - value2;
        break;
    case 2:
        result = value1 * value2;
        break;
    case 3:
        result = value1 / value2;
        break;
    case 4:
        if (value1 > 0 && value2 > 0)
        {
            result = std::pow(value1, value2);
            break;
        }
    case 5:
        if (value1 > 0 && value2 > 0)
        {
            result = std::log(value1) / std::log(value2);
            break;
        }
    case 6:
        result = std::min(value1, value2);
        break;
    case 7:
        result = std::max(value1, value2);
        break;
    default:
        result = value1;
    }

    return {{"out_result", result}};
}