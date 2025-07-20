#pragma once

#include "PGS/node_graph/node.h"

namespace PGS::NodeGraph
{

class VoronoiTextureNode final : public Node {
public:
    VoronoiTextureNode(NodeID id, std::string name);

    std::unordered_map<PortID, NodeData> calculate(
        std::unordered_map<PortID, NodeData>& inputs, const sf::Vector2u& bufferSize) const override;

    enum FeatureType {
        F1 = 0,
        F2 = 1,
        SmoothF1 = 2
    };

    enum DistanceType {
        Euclidean = 0,
        Manhattan = 1,
        Chebyshev = 2
    };
private:
    static float distance(const sf::Vector2f& a, const sf::Vector2f& b, int metric);

    static sf::Color idToColor(size_t id);

    static std::vector<sf::Vector2f> generateFeaturePoints(float scale, float randomness, int seed = 0);
};

} // namespace PGS::NodeGraph