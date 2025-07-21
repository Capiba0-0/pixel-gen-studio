#include "PGS/node_graph/nodes/voronoi_texture_node.h"

#include "PGS/node_graph/helpers.h"
#include "PGS/node_graph/converters.h"

#include <random>


// -- Private --
float PGS::NodeGraph::VoronoiTextureNode::distance(const sf::Vector2f& a, const sf::Vector2f& b, const int metric)
{
    const sf::Vector2f d = a - b;
    if (metric == Euclidean)
        return std::sqrt(d.x * d.x + d.y * d.y);
    if (metric == Manhattan)
        return std::abs(d.x) + std::abs(d.y);
    else // (metric == Chebyshev)
        return std::max(std::abs(d.x), std::abs(d.y));
}

sf::Color PGS::NodeGraph::VoronoiTextureNode::idToColor(const size_t id)
{
    std::mt19937 rng(static_cast<unsigned int>(id));
    std::uniform_int_distribution<int> dist(0, 255);
    return {static_cast<std::uint8_t>(dist(rng)), static_cast<std::uint8_t>(dist(rng)), static_cast<std::uint8_t>(dist(rng))};

}

std::vector<sf::Vector2f> PGS::NodeGraph::VoronoiTextureNode::generateFeaturePoints(
    const float scale, const float randomness, const int seed)
{
    std::vector<sf::Vector2f> points;
    std::mt19937 rng(seed);
    std::uniform_real_distribution jitter(-0.5f, 0.5f);

    const int gridX = static_cast<int>(scale);
    const int gridY = static_cast<int>(scale);

    for (int gy = 0; gy < gridY; ++gy) {
        for (int gx = 0; gx < gridX; ++gx) {
            float px = (static_cast<float>(gx) + 0.5f + randomness * jitter(rng)) / static_cast<float>(gridX);
            float py = (static_cast<float>(gy) + 0.5f + randomness * jitter(rng)) / static_cast<float>(gridY);
            points.emplace_back(px, py);
        }
    }

    return points;
}


// -- Public --
PGS::NodeGraph::VoronoiTextureNode::VoronoiTextureNode(const NodeID id, std::string name)
    : Node(id, std::move(name))
{
    // Input
    registerInputPort({"in_feature", "", DataType::Number, ValueList{0, {"F1", "F2", "Smooth F1"}}});
    registerInputPort({"in_metric", "", DataType::Number, ValueList{0, {"Euclidean", "Manhattan", "Chebyshev"}}});

    registerInputPort({"in_normalize", "Normalize", DataType::Number, true});
    registerInputPort({"in_vector", "Vector", DataType::VectorField});

    registerInputPort({"in_scale", "Scale", DataType::Number, 5.0f});
    registerInputPort({"in_detail", "Detail", DataType::Number, 0.0f});
    registerInputPort({"in_roughness", "Roughness", DataType::Number, 0.5f,
        Metadata{.description = "Roughness of the effect", .minValue = 0.0f, .maxValue = 1.0f}});

    registerInputPort({"in_lacunarity", "Lacunarity", DataType::Number, 2.0f});
    registerInputPort({"in_randomness", "Randomness", DataType::Number, 1.0f,
        Metadata{.description = "Randomness of the effect", .minValue = 0.0f, .maxValue = 1.0f}});

    // Output
    registerOutputPort({"out_grayscale", "Distance", DataType::Grayscale});
    registerOutputPort({"out_color", "Color", DataType::Color});
}

std::unordered_map<PGS::NodeGraph::PortID, PGS::NodeGraph::NodeData> PGS::NodeGraph::VoronoiTextureNode::calculate(
    std::unordered_map<PortID, NodeData>& inputs, const sf::Vector2u& bufferSize) const
{
    auto outGrayscale = std::make_shared<GrayscaleBuffer>(bufferSize);
    auto outColor = std::make_shared<PixelBuffer>(bufferSize);

    const auto feature = static_cast<int>(getRequiredInput<float>(inputs, "in_feature", bufferSize));
    const auto metric = static_cast<int>(getRequiredInput<float>(inputs, "in_metric", bufferSize));
    const bool normalize = static_cast<bool>(getRequiredInput<float>(inputs, "in_normalize", bufferSize));
    const auto scale = getRequiredInput<float>(inputs, "in_scale", bufferSize);
    const auto randomness = getRequiredInput<float>(inputs, "in_randomness", bufferSize);

    std::shared_ptr<VectorFieldBuffer> vectorField = nullptr;
    if (inputs.count("in_vector")) {
        vectorField = getRequiredInput<std::shared_ptr<VectorFieldBuffer>>(inputs, "in_vector", bufferSize);
    }

    const auto points = generateFeaturePoints(scale, randomness);

    std::vector<float> distancesBuffer(bufferSize.x * bufferSize.y);
    std::vector<size_t> closestIDs(bufferSize.x * bufferSize.y);

    float minDist = std::numeric_limits<float>::max();
    float maxDist = std::numeric_limits<float>::lowest();

    for (unsigned int y = 0; y < bufferSize.y; ++y) {
        for (unsigned int x = 0; x < bufferSize.x; ++x) {
            sf::Vector2f coord = {
                static_cast<float>(x) / static_cast<float>(bufferSize.x),
                static_cast<float>(y) / static_cast<float>(bufferSize.y)
            };

            if (vectorField) {
                coord += vectorField->getVector({x, y});
            }

            std::vector<std::pair<float, size_t>> distances;
            distances.reserve(points.size());

            for (size_t i = 0; i < points.size(); ++i) {
                float d = distance(coord, points[i], metric);
                distances.emplace_back(d, i);
            }

            std::sort(distances.begin(), distances.end(),
                      [](const auto& a, const auto& b) { return a.first < b.first; });

            float val = 0.f;
            switch (feature) {
                case F1:
                    val = distances[0].first;
                    break;
                case F2:
                    val = distances.size() > 1 ? distances[1].first : distances[0].first;
                    break;
                case SmoothF1: {
                    float d0 = distances[0].first;
                    float d1 = distances.size() > 1 ? distances[1].first : d0;
                    val = 0.5f * (d0 + d1);
                    break;
                }
                default:
                    val = distances[0].first;
                    break;
            }

            distancesBuffer[y * bufferSize.x + x] = val;
            closestIDs[y * bufferSize.x + x] = distances[0].second;

            minDist = std::min(minDist, val);
            maxDist = std::max(maxDist, val);
        }
    }

    float range = maxDist - minDist;
    if (range < 1e-6f) range = 1.0f;

    for (unsigned int y = 0; y < bufferSize.y; ++y) {
        for (unsigned int x = 0; x < bufferSize.x; ++x) {
            const size_t index = y * bufferSize.x + x;

            float val = distancesBuffer[index];
            if (normalize) {
                val = (val - minDist) / range;
            }
            val = std::clamp(val, 0.f, 1.f);

            const size_t id = closestIDs[index];

            outGrayscale->setValue({x, y}, static_cast<uint8_t>(val * 255));
            outColor->setPixel({x, y}, idToColor(id));
        }
    }

    return {
        {"out_grayscale", std::move(outGrayscale)},
        {"out_color", std::move(outColor)}
    };

}
