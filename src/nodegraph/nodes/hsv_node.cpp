#include <cmath>

#include "PGS/nodegraph/nodes/hsv_node.h"

#include "PGS/nodegraph/helpers.h"
#include "PGS/nodegraph/converters.h"
#include "PGS/nodegraph/utils/lerp.h"

PGS::NodeGraph::HSVNode::HSVNode(const NodeID id, std::string name)
    : Node(id, std::move(name))
{
    // Input
    registerInputPort({"in_hue", "Hue", DataType::Number, 0.5f});
    registerInputPort({"in_saturation", "Saturation", DataType::Number, 1.0f});
    registerInputPort({"in_value", "Value", DataType::Number, 1.0f});

    registerInputPort({"in_fac", "Fac", DataType::Number, 1.0f,
        Metadata{.description = "Factor of the effect", .minValue = 0.0f, .maxValue = 1.0f}});

    registerInputPort({"in_color", "Color", DataType::Color, sf::Color::White});

    // Output
    registerOutputPort({"out_color", "Color", DataType::Color});
}

std::unordered_map<PGS::NodeGraph::PortID, PGS::NodeGraph::NodeData>
PGS::NodeGraph::HSVNode::calculate(std::unordered_map<PortID, NodeData>& inputs, const sf::Vector2u& bufferSize) const
{
    auto outColor = std::make_shared<PixelBuffer>(bufferSize);

    // Отримуємо вхідні дані
    const auto hue = getRequiredInput<float>(inputs, "in_hue", bufferSize);
    const auto saturation = getRequiredInput<float>(inputs, "in_saturation", bufferSize);
    const auto value = getRequiredInput<float>(inputs, "in_value", bufferSize);
    const auto factor = getRequiredInput<float>(inputs, "in_fac", bufferSize);
    const auto colorBuffer = getRequiredInput<std::shared_ptr<PixelBuffer>>(inputs, "in_color", bufferSize);

    for (unsigned int y = 0; y < bufferSize.y; ++y) {
        for (unsigned int x = 0; x < bufferSize.x; ++x) {
            const sf::Vector2u pos = {x, y};

            const sf::Color originalColor = colorBuffer->getPixel(pos);

            HSV hsv = Converters::rgbToHsv(originalColor);

            hsv.h += hue - 0.5f;
            hsv.h = std::fmod(hsv.h, 1.0f);
            if (hsv.h < 0.0f) {
                hsv.h += 1.0f;
            }
            hsv.s *= saturation;
            hsv.v *= value;

            hsv.s = std::clamp(hsv.s, 0.0f, 1.0f);
            hsv.v = std::clamp(hsv.v, 0.0f, 1.0f);

            sf::Color modifiedColor = Converters::hsvToRgb(hsv);
            modifiedColor.a = originalColor.a;

            sf::Color finalColor = Utils::lerpColor(originalColor, modifiedColor, factor);

            outColor->setPixel(pos, finalColor);
        }
    }

    std::unordered_map<PortID, NodeData> results;
    results["out_color"] = std::move(outColor);
    return results;
}