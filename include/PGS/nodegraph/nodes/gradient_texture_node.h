#pragma once

#include "PGS/nodegraph/node.h"

namespace PGS::NodeGraph
{

class GradientTextureNode final : public Node
{
public:
    GradientTextureNode(NodeID id, std::string name);

    std::unordered_map<PortID, NodeData> calculate(
        std::unordered_map<PortID, NodeData>& inputs, const sf::Vector2u& bufferSize) const override;

private:
    enum class GradientType {
        Linear,
        Quadratic,
        Easing,
        Radial,
        Diagonal
    };
};

} // namespace PGS::NodeGraph