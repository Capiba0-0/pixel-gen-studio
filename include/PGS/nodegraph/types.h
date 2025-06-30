#pragma once

#include "SFML/Graphics/Color.hpp"

#include <variant>
#include <memory>
#include <string>
#include <optional>
#include <stdexcept>
#include <unordered_map>

// Declaration
namespace PGS
{
    class GrayscaleBuffer;
    class PixelBuffer;
    class VectorFieldBuffer;
}

namespace PGS::NodeGraph
{

    using NodeID = size_t;
    using PortID = std::string;

    using InputPortValue = std::variant<float, int, bool, sf::Color>;

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


    template <typename T>
    std::optional<T> getNodeDataAs(const NodeData& data)
    {
        if (std::holds_alternative<T>(data))
        {
            return std::get<T>(data);
        }
        return std::nullopt;
    }

    template <typename T>
    T getRequiredInput(const std::unordered_map<PortID, NodeData>& inputs, const PortID& portId)
    {
        auto it = inputs.find(portId);
        if (it == inputs.end())
        {
            throw std::runtime_error("FATAL ERROR: Required input data for port '" + portId + "' was not provided by the evaluator.");
        }

        const NodeData& data = it->second;

        auto valueOpt = getNodeDataAs<T>(data);

        if (valueOpt.has_value())
        {
            return *valueOpt;
        }
        throw std::runtime_error("FATAL ERROR: Type mismatch for input port: " + portId);
    }
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

    // For Hash-Tables (std::unordered_map)
    template<>
    struct hash<PGS::NodeGraph::OutputPortLocator> {
        size_t operator()(const PGS::NodeGraph::OutputPortLocator& key) const noexcept
        {
            const size_t h1 = std::hash<PGS::NodeGraph::NodeID>{}(key.node);
            const size_t h2 = std::hash<PGS::NodeGraph::PortID>{}(key.port);
            return h1 ^ (h2 << 1);
        }
    };

} // namespace std