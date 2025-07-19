#include "PGS/nodegraph/nodes/rectangle_pattern_node.h"

#include "PGS/nodegraph/helpers.h"

PGS::NodeGraph::RectanglePatternNode::RectanglePatternNode(const NodeID id, std::string name)
    : Node(id, std::move(name))
{
    // Input
    registerInputPort({ "in_vector", "Vector", DataType::VectorField });

    registerInputPort({ "in_color", "Color", DataType::Color, sf::Color::Black });

    registerInputPort({ "in_x", "X", DataType::Number, 0 });
    registerInputPort({ "in_y", "Y", DataType::Number, 0 });
    registerInputPort({ "in_size_x", "Size X", DataType::Number, 0 });
    registerInputPort({ "in_size_y", "Size Y", DataType::Number, 0 });

    registerInputPort({ "in_is_filling", "Fill", DataType::Number, false });

    // Output
    registerOutputPort({ "out_color", "Color", DataType::Color });
    registerOutputPort({ "out_grayscale", "Grayscale", DataType::Grayscale });
}

std::unordered_map<PGS::NodeGraph::PortID, PGS::NodeGraph::NodeData> PGS::NodeGraph::RectanglePatternNode::calculate(std::unordered_map<PortID, NodeData>& inputs, const sf::Vector2u& bufferSize) const
{
    auto outColor = std::make_shared<PixelBuffer>(bufferSize);
    auto outGrayscale = std::make_shared<GrayscaleBuffer>(bufferSize);

    std::shared_ptr<VectorFieldBuffer> vectorField = nullptr;
    if (inputs.count("in_vector")) {
        vectorField = getRequiredInput<std::shared_ptr<VectorFieldBuffer>>(inputs, "in_vector", bufferSize);
    }

    const auto colorBuffer = getRequiredInput<std::shared_ptr<PixelBuffer>>(inputs, "in_color", bufferSize);
    const auto rectX = getRequiredInput<float>(inputs, "in_x", bufferSize);
    const auto rectY = getRequiredInput<float>(inputs, "in_y", bufferSize);
    const auto sizeX = getRequiredInput<float>(inputs, "in_size_x", bufferSize);
    const auto sizeY = getRequiredInput<float>(inputs, "in_size_y", bufferSize);
    const bool isFilling = static_cast<bool>(getRequiredInput<float>(inputs, "in_is_filling", bufferSize));

    const float left   = rectX;
    const float right  = rectX + sizeX;
    const float top    = rectY;
    const float bottom = rectY + sizeY;

    for (unsigned y = 0; y < bufferSize.y; ++y) {
        for (unsigned x = 0; x < bufferSize.x; ++x) {
            auto sampleX = static_cast<float>(x);
            auto sampleY = static_cast<float>(y);

            if (vectorField) {
                const sf::Vector2f distortion = vectorField->getVector({x, y});
                sampleX += distortion.x;
                sampleY += distortion.y;
            }

            const bool inside = sampleX >= left && sampleX <= right && sampleY >= top && sampleY <= bottom;
            if (!inside)
                continue;

            bool shouldDraw = isFilling;
            if (!isFilling) {
                const bool onLeft   = std::abs(sampleX - left) < 1;
                const bool onRight  = std::abs(sampleX - right) < 1;
                const bool onTop    = std::abs(sampleY - top) < 1;
                const bool onBottom = std::abs(sampleY - bottom) < 1;
                shouldDraw = onLeft || onRight || onTop || onBottom;
            }

            if (shouldDraw) {
                const sf::Color finalColor = colorBuffer->getPixel({x, y});
                outColor->setPixel({x, y}, finalColor);

                const float luminance = 0.299f * static_cast<float>(finalColor.r) +
                                        0.587f * static_cast<float>(finalColor.g) +
                                        0.114f * static_cast<float>(finalColor.b);
                outGrayscale->setValue({x, y}, static_cast<uint8_t>(luminance));
            }
        }
    }

    return {{"out_color", std::move(outColor)},
            {"out_grayscale", std::move(outGrayscale)}};
}