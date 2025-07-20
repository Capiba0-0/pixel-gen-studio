#include "PGS/node_graph/nodes/combine_xy_node.h"

#include "PGS/node_graph/helpers.h"
#include "PGS/core/buffers/vector_field_buffer.h"
#include "PGS/core/buffers/grayscale_buffer.h"
#include <algorithm>

PGS::NodeGraph::CombineXYNode::CombineXYNode(const NodeID id, std::string name)
    : Node(id, std::move(name))
{
    // Input
    registerInputPort({"in_x", "X", DataType::Grayscale, 0.0f});
    registerInputPort({"in_y", "Y", DataType::Grayscale, 0.0f});

    // Output
    registerOutputPort({"out_vector", "Vector", DataType::VectorField});
}

std::unordered_map<PGS::NodeGraph::PortID, PGS::NodeGraph::NodeData>
PGS::NodeGraph::CombineXYNode::calculate(std::unordered_map<PortID, NodeData>& inputs, const sf::Vector2u& bufferSize) const
{
    auto outVector = std::make_shared<VectorFieldBuffer>(bufferSize);

    const auto xBuffer = getRequiredInput<std::shared_ptr<GrayscaleBuffer>>(inputs, "in_x", bufferSize);
    const auto yBuffer = getRequiredInput<std::shared_ptr<GrayscaleBuffer>>(inputs, "in_y", bufferSize);

    for (unsigned int y = 0; y < bufferSize.y; ++y) {
        for (unsigned int x = 0; x < bufferSize.x; ++x) {
            const sf::Vector2u pos = {x, y};

            const float valueX = static_cast<float>(xBuffer->getValue(pos)) / 255.0f;
            const float valueY = static_cast<float>(yBuffer->getValue(pos)) / 255.0f;

            outVector->setVector(pos, {valueX, valueY});
        }
    }

    return {{"out_vector", std::move(outVector)}};
}
