#pragma once

#include "PGS/nodegraph/node.h"

namespace PGS::NodeGraph
{

class MathNode final : public Node
{
public:
    MathNode(NodeID id, std::string name);

    std::unordered_map<PortID, NodeData> calculate(
        std::unordered_map<PortID, NodeData>& inputs, const sf::Vector2u& bufferSize) const override;
};

} // namespace PGS::NodeGraph