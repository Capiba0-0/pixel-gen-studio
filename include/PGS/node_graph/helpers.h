#pragma once

#include "PGS/node_graph/converters.h"
#include "PGS/node_graph/types.h"

#include <stdexcept>

namespace PGS::NodeGraph
{

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

} // namespace PGS::NodeGraph