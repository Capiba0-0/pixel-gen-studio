#pragma once

#include "PGS/node_graph/node.h"

namespace PGS::NodeGraph
{

    class SeparateXYNode final : public Node
    {
    public:
        SeparateXYNode(NodeID id, std::string name);

        std::unordered_map<PortID, NodeData> calculate(
            std::unordered_map<PortID, NodeData>& inputs, const sf::Vector2u& bufferSize) const override;
    };

} // namespace PGS::NodeGraph