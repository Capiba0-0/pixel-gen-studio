#include "PGS/nodegraph/nodes/circle_pattern_node.h"

#include "PGS/nodegraph/helpers.h"

PGS::NodeGraph::CirclePatternNode::CirclePatternNode(const NodeID id, std::string name)
    : Node(id, std::move(name))
{
    // Input
    registerInputPort({ "in_vector", "Vector", DataType::VectorField });

    registerInputPort({ "in_color", "Color", DataType::Color, sf::Color::Black });

    registerInputPort({ "in_center_x", "Center X", DataType::Number, 0 });
    registerInputPort({ "in_center_y", "Center Y", DataType::Number, 0 });
    registerInputPort({ "in_radius", "Radius", DataType::Number, 0 });

    registerInputPort({ "in_is_filling", "Filling", DataType::Number, false });

    // Output
    registerOutputPort({ "out_color", "Color", DataType::Color });
    registerOutputPort({ "out_grayscale", "Grayscale", DataType::Grayscale });
}

std::unordered_map<PGS::NodeGraph::PortID, PGS::NodeGraph::NodeData> PGS::NodeGraph::CirclePatternNode::calculate(
    std::unordered_map<PortID, NodeData>& inputs, const sf::Vector2u& bufferSize) const
{
    auto outColor = std::make_shared<PixelBuffer>(bufferSize);
    auto outGrayscale = std::make_shared<GrayscaleBuffer>(bufferSize);

    std::shared_ptr<VectorFieldBuffer> vectorField = nullptr;
    if (inputs.count("in_vector")) {
        vectorField = getRequiredInput<std::shared_ptr<VectorFieldBuffer>>(inputs, "in_vector", bufferSize);
    }

    const auto colorBuffer = getRequiredInput<std::shared_ptr<PixelBuffer>>(inputs, "in_color", bufferSize);
    const auto centerX = getRequiredInput<float>(inputs, "in_center_x", bufferSize);
    const auto centerY = getRequiredInput<float>(inputs, "in_center_y", bufferSize);
    const auto radius = getRequiredInput<float>(inputs, "in_radius", bufferSize);
    const bool isFilling = static_cast<bool>(getRequiredInput<float>(inputs, "in_is_filling", bufferSize));


    for (unsigned int y = 0; y < bufferSize.y; ++y) {
        for (unsigned int x = 0; x < bufferSize.x; ++x) {

            auto sampleX = static_cast<float>(x);
            auto sampleY = static_cast<float>(y);

            if (vectorField) {
                sf::Vector2f distortion = vectorField->getVector({x, y});
                sampleX += distortion.x;
                sampleY += distortion.y;
            }

            const float dx = sampleX - centerX;
            const float dy = sampleY - centerY;
            const float distSq = dx * dx + dy * dy;

            bool inside = false;
            if (isFilling) {
                inside = distSq <= (radius + 0.5f) * (radius + 0.5f);
            } else {
                const float minusHalf = radius - 0.5f;
                const float plusHalf = radius + 0.5f;

                inside = (distSq >= minusHalf * minusHalf) && (distSq <= plusHalf * plusHalf);
            }

            if (inside) {
                sf::Color finalColor = colorBuffer->getPixel({x, y});
                outColor->setPixel({x, y}, finalColor);

                const float luminance = 0.299f * static_cast<float>(finalColor.r) +
                                        0.587f * static_cast<float>(finalColor.g) +
                                        0.114f * static_cast<float>(finalColor.b);
                outGrayscale->setValue({x, y}, static_cast<uint8_t>(luminance));
            } else {
                outColor->setPixel({x, y}, sf::Color::Transparent);
                outGrayscale->setValue({x, y}, 0);
            }
        }
    }

    return {{"out_color", std::move(outColor)},
            {"out_grayscale", std::move(outGrayscale)}};
}
