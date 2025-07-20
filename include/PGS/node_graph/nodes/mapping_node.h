#pragma once

#include "PGS/node_graph/node.h"

namespace PGS::NodeGraph
{

class MappingNode final : public Node
{
public:
    MappingNode(NodeID id, std::string name);

    std::unordered_map<PortID, NodeData> calculate(
        std::unordered_map<PortID, NodeData>& inputs, const sf::Vector2u& bufferSize) const override;

private:
    enum class MappingType {
        Point,
        Texture,
        Vector
    };
};

} // namespace PGS::NodeGraph