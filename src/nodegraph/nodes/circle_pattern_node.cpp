#include "PGS/nodegraph/nodes/circle_pattern_node.h"

PGS::NodeGraph::CirclePatternNode::CirclePatternNode(const NodeID id, std::string name)
    : Node(id, std::move(name))
{
    // Input
    registerInputPort({
        .id = "in_vector",
        .name = "Vector",
        .type = DataType::VectorField
    });

    registerInputPort({
        .id = "in_color",
        .name = "Color",
        .type = DataType::Color,
        .value = sf::Color::Black
    });

    registerInputPort({
        .id = "in_center_x",
        .name = "Center X",
        .type = DataType::Number,
        .value = 0
    });

    registerInputPort({
        .id = "in_center_y",
        .name = "Center Y",
        .type = DataType::Number,
        .value = 0
    });

    registerInputPort({
        .id = "in_radius",
        .name = "Radius",
        .type = DataType::Number,
        .value = 0
    });

    registerInputPort({
        .id = "in_is_filling",
        .name = "Filling",
        .type = DataType::Number,
        .value = false
    });

    // Output
    registerOutputPort({
        .id = "out_color",
        .name = "Color",
        .type = DataType::Color
    });

    registerOutputPort({
        .id = "out_grayscale",
        .name = "Grayscale",
        .type = DataType::Grayscale
    });
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

    std::unordered_map<PortID, NodeData> results;
    results["out_color"] = std::move(outColor);
    results["out_grayscale"] = std::move(outGrayscale);
    return results;
}