#pragma once

#include "PGS/nodegraph/node.h"

namespace PGS::NodeGraph
{

class MixColorNode final : public Node
{
public:
    MixColorNode(NodeID id, std::string name);

    std::unordered_map<PortID, NodeData> calculate(
        std::unordered_map<PortID, NodeData>& inputs, const sf::Vector2u& bufferSize) const override;

    enum BlendingMode {
        Mix, Darken, Multiply,
        Lighten, Screen, Add,
        Overlay, SoftLight,
        LinearLight, Difference,
        Exclusion, Subtract, Divide
    };
};

} // namespace PGS::NodeGraph