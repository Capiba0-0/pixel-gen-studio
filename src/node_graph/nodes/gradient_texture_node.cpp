#include "PGS/node_graph/nodes/gradient_texture_node.h"

#include "PGS/core/buffers/pixel_buffer.h"
#include "PGS/core/buffers/grayscale_buffer.h"
#include "PGS/node_graph/helpers.h"

#include <algorithm>

namespace
{
    float smoothstep(float t) {
        t = std::clamp(t, 0.0f, 1.0f);
        return t * t * (3.0f - 2.0f * t);
    }
}

PGS::NodeGraph::GradientTextureNode::GradientTextureNode(const NodeID id, std::string name)
    : Node(id, std::move(name))
{
    // Input
    registerInputPort({"in_vector", "Vector", DataType::VectorField});
    registerInputPort({"in_gradient_type", "Type", DataType::Number,
        ValueList{0, {"Linear", "Quadratic", "Easing", "Radial", "Diagonal"}}});

    // Output
    registerOutputPort({"out_color", "Color", DataType::Color});
    registerOutputPort({"out_grayscale", "Grayscale", DataType::Grayscale});
}

std::unordered_map<PGS::NodeGraph::PortID, PGS::NodeGraph::NodeData>
PGS::NodeGraph::GradientTextureNode::calculate(std::unordered_map<PortID, NodeData>& inputs, const sf::Vector2u& bufferSize) const
{
    auto outColor = std::make_shared<PixelBuffer>(bufferSize);
    auto outGrayscale = std::make_shared<GrayscaleBuffer>(bufferSize);

    const auto gradientTypeIndex = static_cast<int>(getRequiredInput<float>(inputs, "in_gradient_type", bufferSize));
    const auto gradientType = static_cast<GradientType>(std::clamp(gradientTypeIndex, 0, 4));

    std::shared_ptr<VectorFieldBuffer> vectorField = nullptr;
    if (inputs.count("in_vector")) {
        vectorField = getRequiredInput<std::shared_ptr<VectorFieldBuffer>>(inputs, "in_vector", bufferSize);
    }

    for (unsigned int y = 0; y < bufferSize.y; ++y) {
        for (unsigned int x = 0; x < bufferSize.x; ++x) {
            // Coord distortion
            sf::Vector2f coord;

            if (vectorField) {
                coord = vectorField->getVector({x, y});
            } else {
                coord.x = (bufferSize.x > 1) ? static_cast<float>(x) / static_cast<float>(bufferSize.x - 1) : 0.0f;
                coord.y = (bufferSize.y > 1) ? static_cast<float>(y) / static_cast<float>(bufferSize.y - 1) : 0.0f;
            }

            // Processing gradient type
            float t = 0.0f;
            switch (gradientType)
            {
                case GradientType::Linear:
                    t = coord.x;
                    break;
                case GradientType::Quadratic:
                    t = coord.x * coord.x;
                    break;
                case GradientType::Easing:
                    t = smoothstep(coord.x);
                    break;
                case GradientType::Radial: {
                        float dx = coord.x - 0.5f;
                        float dy = coord.y - 0.5f;
                        t = std::sqrt(dx * dx + dy * dy) * 2.0f;
                        break;
                }
                case GradientType::Diagonal:
                    t = (coord.x + coord.y) / 2.0f;
                    break;
            }

            t = std::clamp(t, 0.0f, 1.0f);
            const auto byteValue = static_cast<uint8_t>(t * 255.0f);

            outGrayscale->setValue({x, y}, byteValue);
            outColor->setPixel({x, y}, sf::Color(byteValue, byteValue, byteValue));
        }
    }

    return {
        {"out_color", std::move(outColor)},
        {"out_grayscale", std::move(outGrayscale)}
    };
}
