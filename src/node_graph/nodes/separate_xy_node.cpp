#include "PGS/node_graph/nodes/separate_xy_node.h"

#include "PGS/node_graph/helpers.h"
#include "PGS/core/buffers/grayscale_buffer.h"
#include <algorithm>

PGS::NodeGraph::SeparateXYNode::SeparateXYNode(const NodeID id, std::string name)
    : Node(id, std::move(name))
{
    // Input
    registerInputPort({"in_vector", "Vector", DataType::VectorField});

    // Output
    registerOutputPort({"out_x", "X", DataType::Grayscale});
    registerOutputPort({"out_y", "Y", DataType::Grayscale});
}

std::unordered_map<PGS::NodeGraph::PortID, PGS::NodeGraph::NodeData> 
PGS::NodeGraph::SeparateXYNode::calculate(std::unordered_map<PortID, NodeData>& inputs, const sf::Vector2u& bufferSize) const
{
    auto outX = std::make_shared<GrayscaleBuffer>(bufferSize);
    auto outY = std::make_shared<GrayscaleBuffer>(bufferSize);

    std::shared_ptr<VectorFieldBuffer> inVector = nullptr;
    if (inputs.count("in_vector")) {
        inVector = getRequiredInput<std::shared_ptr<VectorFieldBuffer>>(inputs, "in_vector", bufferSize);
    }

    for (unsigned int y = 0; y < bufferSize.y; ++y) {
        for (unsigned int x = 0; x < bufferSize.x; ++x) {
            const sf::Vector2u pos = {x, y};

            sf::Vector2f vec = {0.0f, 0.0f};
            if (inVector) {
                vec = inVector->getVector(pos);
            }

            const uint8_t valueX = static_cast<uint8_t>(std::clamp(vec.x, 0.0f, 1.0f) * 255.0f);
            const uint8_t valueY = static_cast<uint8_t>(std::clamp(vec.y, 0.0f, 1.0f) * 255.0f);
            
            outX->setValue(pos, valueX);
            outY->setValue(pos, valueY);
        }
    }

    return {
        {"out_x", std::move(outX)},
        {"out_y", std::move(outY)}
    };
}
