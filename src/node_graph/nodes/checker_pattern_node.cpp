#include "PGS/node_graph/nodes/checker_pattern_node.h"

#include "PGS/node_graph/helpers.h"

#include "PGS/core/buffers/pixel_buffer.h"
#include "PGS/core/buffers/grayscale_buffer.h"
#include "PGS/core/buffers/vector_field_buffer.h"

#include <memory>
#include <cmath>

PGS::NodeGraph::CheckerPatternNode::CheckerPatternNode(const NodeID id, std::string name)
    : Node(id, std::move(name))
{
    // Input
    registerInputPort({ "in_vector", "Vector", DataType::VectorField });

    registerInputPort({ "in_color1", "Color1", DataType::Color, sf::Color::White });
    registerInputPort({ "in_color2", "Color2", DataType::Color, sf::Color::Black });

    registerInputPort({ "in_scale", "Scale", DataType::Number, 1 });

    // Output
    registerOutputPort({ "out_color", "Color", DataType::Color });
    registerOutputPort({ "out_grayscale", "Grayscale", DataType::Grayscale });
}

std::unordered_map<PGS::NodeGraph::PortID, PGS::NodeGraph::NodeData> PGS::NodeGraph::CheckerPatternNode::calculate(
    std::unordered_map<PortID, NodeData>& inputs, const sf::Vector2u& bufferSize) const
{
    auto outColor = std::make_shared<PixelBuffer>(bufferSize);
    auto outGrayscale = std::make_shared<GrayscaleBuffer>(bufferSize);

    // Getting port values
    std::shared_ptr<VectorFieldBuffer> vectorField = nullptr;
    if (inputs.find("in_vector") != inputs.end())
        vectorField = getRequiredInput<std::shared_ptr<VectorFieldBuffer>>(inputs, "in_vector", bufferSize);

    const auto firstColor = getRequiredInput<std::shared_ptr<PixelBuffer>>(inputs, "in_color1", bufferSize);
    const auto secondColor = getRequiredInput<std::shared_ptr<PixelBuffer>>(inputs, "in_color2", bufferSize);
    const auto scale = static_cast<int>(getRequiredInput<float>(inputs, "in_scale", bufferSize));

    // Main algorithm
    sf::Color finalColor;

    for (unsigned y = 0; y < bufferSize.y; ++y) {
        for (unsigned x = 0; x < bufferSize.x; ++x)
        {
            unsigned distortedX = x;
            unsigned distortedY = y;

            if (vectorField != nullptr)
            {
                const sf::Vector2f vecDistortion = vectorField->getVector({x, y});
                distortedX += static_cast<int>(vecDistortion.x);
                distortedY += static_cast<int>(vecDistortion.y);
            }

            const int cellX = std::floor(static_cast<float>(distortedX) / static_cast<float>(scale));
            const int cellY = std::floor(static_cast<float>(distortedY) / static_cast<float>(scale));

            if ((cellX + cellY) % 2 == 0)
            {
                finalColor = firstColor->getPixel({x, y});
            }
            else
            {
                finalColor = secondColor->getPixel({x, y});
            }

            const auto finalGrayscaleValue = static_cast<uint8_t>(0.299 * finalColor.r + 0.587 * finalColor.g + 0.114 * finalColor.b);

            outColor->setPixel({x, y}, finalColor);
            outGrayscale->setValue({x, y}, finalGrayscaleValue);
        }
    }

    std::unordered_map<PortID, NodeData> results;
    results["out_color"] = std::move(outColor);
    results["out_grayscale"] = std::move(outGrayscale);
    return results;
}