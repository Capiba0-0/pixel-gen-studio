#pragma once

#include <SFML/Graphics/Color.hpp>

#include <variant>
#include <memory>
#include <string>
#include <optional>
#include <functional>

// Declaration
namespace PGS
{
    class GrayscaleBuffer;
    class PixelBuffer;
    class VectorFieldBuffer;
}

namespace PGS::NodeGraph
{
    struct HSV { float h, s, v; };


    using NodeID = size_t;
    using PortID = std::string;

    using ValueList = std::pair<int, std::vector<const char*>>;

    using InputPortValue = std::variant<float, int, bool, sf::Color, ValueList>;

    inline constexpr NodeID INVALID_NODE_ID = 0;

    enum class DataType
    {
        Color,
        Grayscale,
        VectorField,
        Number
    };

    struct Metadata
    {
        std::string description;
        float minValue;
        float maxValue;
    };

    struct InputPort
    {
        PortID id;
        std::string name;
        DataType type;

        std::optional<InputPortValue> value;

        Metadata metadata;
    };

    struct OutputPort
    {
        PortID id;
        std::string name;
        DataType type;
    };

    struct Connection
    {
        NodeID sourceNodeId;
        PortID sourcePortId;
        NodeID targetNodeId;
        PortID targetPortId;

        bool operator==(const Connection& other) const {
            return sourceNodeId == other.sourceNodeId &&
                   sourcePortId == other.sourcePortId &&
                   targetNodeId == other.targetNodeId &&
                   targetPortId == other.targetPortId;
        }
    };

    using NodeData = std::variant<
        std::shared_ptr<GrayscaleBuffer>,
        std::shared_ptr<PixelBuffer>,
        std::shared_ptr<VectorFieldBuffer>,
        float
    >;

    // Helpers
    struct InputPortLocator
    {
        NodeID node = INVALID_NODE_ID;
        PortID port;

        // For Hash-Tables (std::unordered_map)
        bool operator==(const InputPortLocator& other) const {
            return node == other.node && port == other.port;
        }
    };

    struct OutputPortLocator
    {
        NodeID node = INVALID_NODE_ID;
        PortID port;

        // For Hash-Tables (std::unordered_map)
        bool operator==(const OutputPortLocator& other) const {
            return node == other.node && port == other.port;
        }
    };
}

namespace std {

    // For Hash-Tables (std::unordered_map)
    template<>
    struct hash<PGS::NodeGraph::InputPortLocator> {
        size_t operator()(const PGS::NodeGraph::InputPortLocator& key) const noexcept
        {
            const size_t h1 = std::hash<PGS::NodeGraph::NodeID>{}(key.node);
            const size_t h2 = std::hash<PGS::NodeGraph::PortID>{}(key.port);
            return h1 ^ (h2 << 1);
        }
    };

    template<>
    struct hash<PGS::NodeGraph::OutputPortLocator> {
        size_t operator()(const PGS::NodeGraph::OutputPortLocator& key) const noexcept
        {
            const size_t h1 = std::hash<PGS::NodeGraph::NodeID>{}(key.node);
            const size_t h2 = std::hash<PGS::NodeGraph::PortID>{}(key.port);
            return h1 ^ (h2 << 1);
        }
    };

    template<>
    struct hash<PGS::NodeGraph::Connection> {
        size_t operator()(const PGS::NodeGraph::Connection& c) const noexcept
        {
            const size_t h1 = std::hash<PGS::NodeGraph::NodeID>{}(c.sourceNodeId);
            const size_t h2 = std::hash<PGS::NodeGraph::PortID>{}(c.sourcePortId);
            const size_t h3 = std::hash<PGS::NodeGraph::NodeID>{}(c.targetNodeId);
            const size_t h4 = std::hash<PGS::NodeGraph::PortID>{}(c.targetPortId);

            return h1 ^ (h2 << 1) ^ (h3 << 2) ^ (h4 << 3);
        }
    };

} // namespace std