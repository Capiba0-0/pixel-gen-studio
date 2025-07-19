#include "PGS/nodegraph/nodes/mix_color_node.h"

#include "PGS/nodegraph/helpers.h"
#include "PGS/nodegraph/utils/lerp.h"

// This is a better practice than making helpers private
namespace
{
    sf::Color blendPixel(const sf::Color& base, const sf::Color& blend, const PGS::NodeGraph::MixColorNode::BlendingMode mode)
    {
        const float baseR = static_cast<float>(base.r) / 255.f;
        const float baseG = static_cast<float>(base.g) / 255.f;
        const float baseB = static_cast<float>(base.b) / 255.f;

        const float topR = static_cast<float>(blend.r) / 255.f;
        const float topG = static_cast<float>(blend.g) / 255.f;
        const float topB = static_cast<float>(blend.b) / 255.f;

        float resultR = 0, resultG = 0, resultB = 0;

        switch (mode)
        {
            case PGS::NodeGraph::MixColorNode::Mix:
                resultR = topR; resultG = topG; resultB = topB;
                break;
            case PGS::NodeGraph::MixColorNode::Darken:
                resultR = std::min(baseR, topR); resultG = std::min(baseG, topG); resultB = std::min(baseB, topB);
                break;
            case PGS::NodeGraph::MixColorNode::Multiply:
                resultR = baseR * topR; resultG = baseG * topG; resultB = baseB * topB;
                break;
            case PGS::NodeGraph::MixColorNode::Lighten:
                resultR = std::max(baseR, topR); resultG = std::max(baseG, topG); resultB = std::max(baseB, topB);
                break;
            case PGS::NodeGraph::MixColorNode::Screen:
                resultR = 1.f - (1.f - baseR) * (1.f - topR);
                resultG = 1.f - (1.f - baseG) * (1.f - topG);
                resultB = 1.f - (1.f - baseB) * (1.f - topB);
                break;
            case PGS::NodeGraph::MixColorNode::Add:
                resultR = baseR + topR; resultG = baseG + topG; resultB = baseB + topB;
                break;
            case PGS::NodeGraph::MixColorNode::Overlay:
                resultR = (baseR < 0.5f) ? (2.f * baseR * topR) : (1.f - 2.f * (1.f - baseR) * (1.f - topR));
                resultG = (baseG < 0.5f) ? (2.f * baseG * topG) : (1.f - 2.f * (1.f - baseG) * (1.f - topG));
                resultB = (baseB < 0.5f) ? (2.f * baseB * topB) : (1.f - 2.f * (1.f - baseB) * (1.f - topB));
                break;
            case PGS::NodeGraph::MixColorNode::SoftLight:
                resultR = (topR < 0.5f) ? (2.f * baseR * topR + baseR * baseR * (1.f - 2.f * topR)) : (2.f * baseR * (1.f - topR) + std::sqrt(baseR) * (2.f * topR - 1.f));
                resultG = (topG < 0.5f) ? (2.f * baseG * topG + baseG * baseG * (1.f - 2.f * topG)) : (2.f * baseG * (1.f - topG) + std::sqrt(baseG) * (2.f * topG - 1.f));
                resultB = (topB < 0.5f) ? (2.f * baseB * topB + baseB * baseB * (1.f - 2.f * topB)) : (2.f * baseB * (1.f - topB) + std::sqrt(baseB) * (2.f * topB - 1.f));
                break;
            case PGS::NodeGraph::MixColorNode::LinearLight:
                resultR = (topR < 0.5f) ? (baseR + 2.f * topR - 1.f) : (baseR + 2.f * (topR - 0.5f));
                resultG = (topG < 0.5f) ? (baseG + 2.f * topG - 1.f) : (baseG + 2.f * (topG - 0.5f));
                resultB = (topB < 0.5f) ? (baseB + 2.f * topB - 1.f) : (baseB + 2.f * (topB - 0.5f));
                break;
            case PGS::NodeGraph::MixColorNode::Difference:
                resultR = std::abs(baseR - topR); resultG = std::abs(baseG - topG); resultB = std::abs(baseB - topB);
                break;
            case PGS::NodeGraph::MixColorNode::Exclusion:
                resultR = baseR + topR - 2.f * baseR * topR;
                resultG = baseG + topG - 2.f * baseG * topG;
                resultB = baseB + topB - 2.f * baseB * topB;
                break;
            case PGS::NodeGraph::MixColorNode::Subtract:
                resultR = baseR - topR; resultG = baseG - topG; resultB = baseB - topB;
                break;
            case PGS::NodeGraph::MixColorNode::Divide:
                resultR = (topR == 0) ? 1.f : baseR / topR;
                resultG = (topG == 0) ? 1.f : baseG / topG;
                resultB = (topB == 0) ? 1.f : baseB / topB;
                break;
        }

        return {
            static_cast<uint8_t>(std::clamp(resultR, 0.0f, 1.0f) * 255.f),
            static_cast<uint8_t>(std::clamp(resultG, 0.0f, 1.0f) * 255.f),
            static_cast<uint8_t>(std::clamp(resultB, 0.0f, 1.0f) * 255.f),
            base.a
        };
    }
}

PGS::NodeGraph::MixColorNode::MixColorNode(const NodeID id, std::string name)
    : Node(id, std::move(name))
{
    // Input
    registerInputPort({"in_blending_mode", "", DataType::Number,
        ValueList{0, {"Mix", "Darken", "Multiply", "Lighten", "Screen", "Add", "Overlay", "Soft Light",
            "Linear Light", "Difference", "Exclusion", "Subtract", "Divide"}}});

    registerInputPort({"in_factor", "Factor", DataType::Grayscale, 0.0f,
        Metadata{.description = "Factor of the effect", .minValue = 0.0f, .maxValue = 1.0f}});

    registerInputPort({"in_color1", "Color1", DataType::Color, sf::Color::White});
    registerInputPort({"in_color2", "Color2", DataType::Color, sf::Color::Black});

    // Output
    registerOutputPort({"out_result", "Result", DataType::Color});
}

std::unordered_map<PGS::NodeGraph::PortID, PGS::NodeGraph::NodeData> PGS::NodeGraph::MixColorNode::calculate(std::unordered_map<PortID, NodeData>& inputs, const sf::Vector2u& bufferSize) const
{
    auto outResult = std::make_shared<PixelBuffer>(bufferSize);

    const auto modeIndex = static_cast<int>(getRequiredInput<float>(inputs, "in_blending_mode", bufferSize));
    const auto blendingMode = static_cast<BlendingMode>(std::clamp(modeIndex, 0, 12));

    const auto factorBuffer = getRequiredInput<std::shared_ptr<GrayscaleBuffer>>(inputs, "in_factor", bufferSize);
    const auto color1Buffer = getRequiredInput<std::shared_ptr<PixelBuffer>>(inputs, "in_color1", bufferSize);
    const auto color2Buffer = getRequiredInput<std::shared_ptr<PixelBuffer>>(inputs, "in_color2", bufferSize);

    for (unsigned int y = 0; y < bufferSize.y; ++y) {
        for (unsigned int x = 0; x < bufferSize.x; ++x) {
            const sf::Vector2u pos = {x, y};

            const sf::Color baseColor = color1Buffer->getPixel(pos);
            const sf::Color blendColor = color2Buffer->getPixel(pos);
            const float factor = static_cast<float>(factorBuffer->getValue(pos)) / 255.0f;

            const sf::Color blendedResult = blendPixel(baseColor, blendColor, blendingMode);

            const sf::Color finalColor = Utils::lerpColor(baseColor, blendedResult, factor);

            outResult->setPixel(pos, finalColor);
        }
    }

    return {{"out_result", std::move(outResult)}};
}
