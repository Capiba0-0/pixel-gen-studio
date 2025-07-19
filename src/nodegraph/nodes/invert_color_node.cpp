#include "PGS/nodegraph/nodes/invert_color_node.h"

#include "PGS/nodegraph/helpers.h"
#include "PGS/nodegraph/utils/lerp.h"

PGS::NodeGraph::InvertColorNode::InvertColorNode(const NodeID id, std::string name)
    : Node(id, std::move(name))
{
    // Input
    registerInputPort({"in_factor", "Factor", DataType::Grayscale, 0.0f,
        Metadata{.description = "Factor of the effect", .minValue = 0.0f, .maxValue = 1.0f}});

    registerInputPort({"in_color", "Color", DataType::Color});

    // Output
    registerOutputPort({"out_color", "Color", DataType::Color});
}

std::unordered_map<PGS::NodeGraph::PortID, PGS::NodeGraph::NodeData> PGS::NodeGraph::InvertColorNode::calculate(std::unordered_map<PortID, NodeData>& inputs, const sf::Vector2u& bufferSize) const
{
    auto outColor = std::make_shared<PixelBuffer>(bufferSize);

    const auto factorBuffer = getRequiredInput<std::shared_ptr<GrayscaleBuffer>>(inputs, "in_factor", bufferSize);
    const auto colorBuffer = getRequiredInput<std::shared_ptr<PixelBuffer>>(inputs, "in_color", bufferSize);

    for (unsigned int y = 0; y < bufferSize.y; ++y) {
        for (unsigned int x = 0; x < bufferSize.x; ++x) {
            const sf::Vector2u pos = {x, y};

            const sf::Color originalColor = colorBuffer->getPixel(pos);
            const float factor = static_cast<float>(factorBuffer->getValue(pos)) / 255.0f;

            const sf::Color invertedColor = {
                static_cast<uint8_t>(255 - originalColor.r),
                static_cast<uint8_t>(255 - originalColor.g),
                static_cast<uint8_t>(255 - originalColor.b),
                originalColor.a
            };

            const sf::Color finalColor = Utils::lerpColor(originalColor, invertedColor, factor);

            outColor->setPixel(pos, finalColor);
        }
    }

    return {{"out_color", std::move(outColor)}};
}
