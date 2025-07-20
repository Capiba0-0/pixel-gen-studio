#include "PGS/node_graph/nodes/noise_texture_node.h"

#include "PGS/node_graph/helpers.h"
#include "PGS/node_graph/converters.h"
#include "PGS/node_graph/utils/perlin_noise_2d.h"

PGS::NodeGraph::NoiseTextureNode::NoiseTextureNode(const NodeID id, std::string name)
    : Node(id, std::move(name))
{
    // Input
    registerInputPort({ "in_normalize", "Normalize", DataType::Number, true });

    registerInputPort({ "in_vector", "Vector", DataType::VectorField });

    registerInputPort({ "in_scale", "Scale", DataType::Number, 5.0f });
    registerInputPort({ "in_detail", "Detail", DataType::Number, 2.0f });

    registerInputPort({ "in_roughness", "Roughness", DataType::Number, 0.5f,
        Metadata{.description = "Roughness of the effect", .minValue = 0.0f, .maxValue = 1.0f}});

    registerInputPort({ "in_lacunarity", "Lacunarity", DataType::Number, 2.0f });
    registerInputPort({ "in_distortion", "Distortion", DataType::Number, 0.0f });

    // Output
    registerOutputPort({ "out_grayscale", "Grayscale", DataType::Grayscale });
    registerOutputPort({ "out_color", "Color", DataType::Color });
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
            Utils::PerlinNoise2D perlin;

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

            sf::Color color = Converters::hsvToRgb({val, 1.0f, 1.0f});
            outColor->setPixel({x, y}, color);
        }
    }

    return {{"out_color", std::move(outColor)},
            {"out_grayscale", std::move(outGrayscale)}};
}