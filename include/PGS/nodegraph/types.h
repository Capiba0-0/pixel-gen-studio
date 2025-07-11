#pragma once

#include <functional>

#include "PGS/nodegraph/converters.h"

#include <SFML/Graphics/Color.hpp>
#include <SFML/System/Vector2.hpp>

#include <variant>
#include <memory>
#include <string>
#include <optional>
#include <stdexcept>
#include <unordered_map>
#include <type_traits>

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


    template <typename T>
    std::optional<T> getNodeDataAs(const NodeData& data)
    {
        if (std::holds_alternative<T>(data))
        {
            return std::get<T>(data);
        }
        return std::nullopt;
    }

    inline bool canConvert(const DataType from, const DataType to)
    {
        if (from == to)
            return true;

        switch (from)
        {
            case DataType::Color:
                return (to == DataType::Grayscale || to == DataType::VectorField);
            case DataType::Grayscale:
                return (to == DataType::Color || to == DataType::VectorField || to == DataType::Number);
            case DataType::VectorField:
                return false;
            case DataType::Number:
                return (to == DataType::Color || to == DataType::Grayscale);
        }

        return false;
    }


    template<typename T>
    std::optional<T> convertTo(const NodeData& sourceData, const sf::Vector2u& targetSize)
    {
        using namespace PGS::NodeGraph::Converters;

        return std::visit([&](auto&& arg) -> std::optional<T>
        {
            using ArgType = std::decay_t<decltype(arg)>;

            if constexpr (std::is_same_v<ArgType, T>) {
                return arg;
            }

            // -- Goal: Grayscale --
            if constexpr (std::is_same_v<T, std::shared_ptr<GrayscaleBuffer>>)
            {
                if constexpr (std::is_same_v<ArgType, std::shared_ptr<PixelBuffer>>) {
                    return toGrayscale(arg);
                }
                if constexpr (std::is_same_v<ArgType, float>) {
                    return toGrayscale(arg, targetSize);
                }
            }

            // -- Goal: PixelBuffer --
            if constexpr (std::is_same_v<T, std::shared_ptr<PixelBuffer>>) {
                if constexpr (std::is_same_v<ArgType, std::shared_ptr<GrayscaleBuffer>>) {
                    return toPixel(arg);
                }
                if constexpr (std::is_same_v<ArgType, float>) {
                    return toPixel(arg, targetSize);
                }
            }

            // -- Goal: VectorFieldBuffer --
            if constexpr (std::is_same_v<T, std::shared_ptr<VectorFieldBuffer>>) {
                if constexpr (std::is_same_v<ArgType, std::shared_ptr<GrayscaleBuffer>>) {
                    return toVectorField(arg);
                }
                if constexpr (std::is_same_v<ArgType, std::shared_ptr<PixelBuffer>>) {
                    return toVectorField(arg);
                }
            }

            // -- Goal: float --
            if constexpr (std::is_same_v<T, float>) {
                if constexpr (std::is_same_v<ArgType, std::shared_ptr<GrayscaleBuffer>>) {
                    return toFloat(arg);
                }
            }

            return std::nullopt;
        }, sourceData);
    }

    template <typename T>
    std::optional<T> getConvertedNodeData(const NodeData& data, const sf::Vector2u& bufferSize,
        std::function<T()> extremeFunction = nullptr)
    {
        auto result = getNodeDataAs<T>(data);
        if (result)
        {
            return result;
        }

        result = convertTo<T>(data, bufferSize);
        if (result)
        {
            return result;
        }

        if (extremeFunction)
        {
            return extremeFunction();
        }

        return std::nullopt;
    }

    template <typename T>
    T getRequiredInput(const std::unordered_map<PortID, NodeData>& inputs, const PortID& portId, const sf::Vector2u& bufferSize)
    {
        const auto it = inputs.find(portId);
        if (it == inputs.end())
        {
            throw std::runtime_error("FATAL ERROR: Required input data for port '" + portId + "' was not provided by the evaluator.");
        }

        const NodeData& data = it->second;

        auto valueOpt = getConvertedNodeData<T>(data, bufferSize);

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