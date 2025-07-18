#include "PGS/nodegraph/nodes/noise_texture_node.h"

#include "PGS/nodegraph/converters.h"
#include "PGS/nodegraph/utils/perlin_noise_2d.h"

PGS::NodeGraph::NoiseTextureNode::NoiseTextureNode(const NodeID id, std::string name)
    : Node(id, std::move(name))
{
    // Input
    registerInputPort({
        .id = "in_normalize",
        .name = "Normalize",
        .type = DataType::Number,
        .value = true
    });

    registerInputPort({
        .id = "in_vector",
        .name = "Vector",
        .type = DataType::VectorField
    });

    registerInputPort({
        .id = "in_scale",
        .name = "Scale",
        .type = DataType::Number,
        .value = 5.0f
    });

    registerInputPort({
        .id = "in_detail",
        .name = "Detail",
        .type = DataType::Number,
        .value = 2.0f
    });

    registerInputPort({
        .id = "in_roughness",
        .name = "Roughness",
        .type = DataType::Number,
        .value = 0.5f
    });

    registerInputPort({
        .id = "in_lacunarity",
        .name = "Lacunarity",
        .type = DataType::Number,
        .value = 2.0f
    });

    registerInputPort({
        .id = "in_distortion",
        .name = "Distortion",
        .type = DataType::Number,
        .value = 0.0f
    });

    // Output
    registerOutputPort({
        .id = "out_grayscale",
        .name = "Grayscale",
        .type = DataType::Grayscale
    });

    registerOutputPort({
        .id = "out_color",
        .name = "Color",
        .type = DataType::Color
    });
}

std::unordered_map<PGS::NodeGraph::PortID, PGS::NodeGraph::NodeData> PGS::NodeGraph::NoiseTextureNode::calculate(
    std::unordered_map<PortID, NodeData>& inputs, const sf::Vector2u& bufferSize) const
{
    auto outGrayscale = std::make_shared<GrayscaleBuffer>(bufferSize);
    auto outColor = std::make_shared<PixelBuffer>(bufferSize);

    std::shared_ptr<VectorFieldBuffer> vectorField = nullptr;
    if (inputs.count("in_vector")) {
        vectorField = getRequiredInput<std::shared_ptr<VectorFieldBuffer>>(inputs, "in_vector", bufferSize);
    }

    const bool isNormalize     = static_cast<bool>(getRequiredInput<float>(inputs, "in_normalize", bufferSize));
    const auto scale      = getRequiredInput<float>(inputs, "in_scale", bufferSize);
    const auto detail     = getRequiredInput<float>(inputs, "in_detail", bufferSize);
    const auto roughness  = getRequiredInput<float>(inputs, "in_roughness", bufferSize);
    const auto lacunarity = getRequiredInput<float>(inputs, "in_lacunarity", bufferSize);
    const auto distortion = getRequiredInput<float>(inputs, "in_distortion", bufferSize);

    std::vector<float> rawNoise(bufferSize.x * bufferSize.y);

    for (unsigned int y = 0; y < bufferSize.y; ++y) {
        for (unsigned int x = 0; x < bufferSize.x; ++x) {
            PerlinNoise2D perlin;

            sf::Vector2f coord = {
                static_cast<float>(x) / static_cast<float>(bufferSize.x),
                static_cast<float>(y) / static_cast<float>(bufferSize.y)
            };

            if (vectorField) {
                coord += vectorField->getVector({x, y});
            }

            coord += distortion * perlin.getValue(coord * 4.0f) * sf::Vector2f{1.f, 1.f};

            float amplitude = 1.0f;
            float frequency = scale;
            float value = 0.0f;

            const int intDetail = static_cast<int>(std::floor(detail));
            const float fracDetail = detail - static_cast<float>(intDetail);

            for (int octave = 0; octave < intDetail; ++octave) {
                value += amplitude * perlin.getValue(coord * frequency);
                frequency *= lacunarity;
                amplitude *= roughness;
            }

            if (fracDetail > 0.f) {
                value += fracDetail * amplitude * perlin.getValue(coord * frequency);
            }

            rawNoise[y * bufferSize.x + x] = value;
        }
    }

    // Normalizing
    const float minVal = *std::min_element(rawNoise.begin(), rawNoise.end());
    const float maxVal = *std::max_element(rawNoise.begin(), rawNoise.end());
    float range = maxVal - minVal;
    if (range < 1e-7f) range = 1.0f;

    for (unsigned int y = 0; y < bufferSize.y; ++y) {
        for (unsigned int x = 0; x < bufferSize.x; ++x) {
            float val = rawNoise[y * bufferSize.x + x];
            if (isNormalize) {
                val = (val - minVal) / range;
            } else {
                val = std::max(0.0f, val);
            }

            val = std::clamp(val, 0.0f, 1.0f);
            const auto grayValue = static_cast<uint8_t>(val * 255);

            outGrayscale->setValue({x, y}, grayValue);

            sf::Color color = Converters::hsvToRgb(val, 1.0f, 1.0f);
            outColor->setPixel({x, y}, color);
        }
    }

    std::unordered_map<PortID, NodeData> results;
    results["out_color"] = std::move(outColor);
    results["out_grayscale"] = std::move(outGrayscale);
    return results;
}