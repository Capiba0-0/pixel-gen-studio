#pragma once

#include "PGS/core/buffers/pixel_buffer.h"
#include "PGS/core/buffers/grayscale_buffer.h"
#include "PGS/core/buffers/vector_field_buffer.h"
#include <memory>
#include <algorithm>
#include <cmath>

namespace PGS::NodeGraph::Converters
{
    // @brief Converts PixelBuffer to GrayscaleBuffer.
    inline std::shared_ptr<GrayscaleBuffer> toGrayscale(const std::shared_ptr<PixelBuffer>& pixelBuffer)
    {
        if (!pixelBuffer) return nullptr;
        const auto size = pixelBuffer->getSize();
        auto grayscaleBuffer = std::make_shared<GrayscaleBuffer>(size);

        for (unsigned int y = 0; y < size.y; ++y) {
            for (unsigned int x = 0; x < size.x; ++x) {
                const sf::Color color = pixelBuffer->getPixel({x, y});

                const float luminance = 0.299f * static_cast<float>(color.r) +
                                        0.587f * static_cast<float>(color.g) +
                                        0.114f * static_cast<float>(color.b);
                grayscaleBuffer->setValue({x, y}, static_cast<uint8_t>(luminance));
            }
        }
        return grayscaleBuffer;
    }

    // @brief Creates a GrayscaleBuffer of the specified size (`size`), filled with one value (`value`).
    inline std::shared_ptr<GrayscaleBuffer> toGrayscale(const float value, const sf::Vector2u& size)
    {
        auto grayscaleBuffer = std::make_shared<GrayscaleBuffer>(size);
        const uint8_t byteValue = static_cast<uint8_t>(std::clamp(value, 0.0f, 1.0f) * 255.0f);

        // TODO: Maybe it will be better to make this func friend for class GrayscaleBuffer instead of using const_cast
        auto* dataPtr = const_cast<uint8_t*>(grayscaleBuffer->getData());
        std::fill_n(dataPtr, (size.x * size.y), byteValue);

        return grayscaleBuffer;
    }

    // @brief Converts GrayscaleBuffer to PixelBuffer.
    inline std::shared_ptr<PixelBuffer> toPixel(const std::shared_ptr<GrayscaleBuffer>& grayscaleBuffer)
    {
        if (!grayscaleBuffer) return nullptr;
        const auto size = grayscaleBuffer->getSize();
        auto pixelBuffer = std::make_shared<PixelBuffer>(size);

        for (unsigned int y = 0; y < size.y; ++y) {
            for (unsigned int x = 0; x < size.x; ++x) {
                const uint8_t value = grayscaleBuffer->getValue({x, y});

                pixelBuffer->setPixel({x, y}, sf::Color(value, value, value));
            }
        }
        return pixelBuffer;
    }

    // @brief Creates a PixelBuffer of the specified size (`size`), filled with one color (`value`).
    inline std::shared_ptr<PixelBuffer> toPixel(const float value, const sf::Vector2u& size)
    {
        auto pixelBuffer = std::make_shared<PixelBuffer>(size);
        const uint8_t byteValue = static_cast<uint8_t>(std::clamp(value, 0.0f, 1.0f) * 255.0f);
        pixelBuffer->clear(sf::Color(byteValue, byteValue, byteValue));
        return pixelBuffer;
    }

    // @brief Converts GrayscaleBuffer to VectorFieldBuffer.
    inline std::shared_ptr<VectorFieldBuffer> toVectorField(const std::shared_ptr<GrayscaleBuffer>& grayscaleBuffer)
    {
        if (!grayscaleBuffer) return nullptr;
        const auto size = grayscaleBuffer->getSize();
        auto vectorField = std::make_shared<VectorFieldBuffer>(size);

        for (unsigned int y = 0; y < size.y; ++y) {
            for (unsigned int x = 0; x < size.x; ++x) {
                const float value = static_cast<float>(grayscaleBuffer->getValue({x, y})) / 255.f;
                vectorField->setVector({x, y}, {0.f, -value});
            }
        }
        return vectorField;
    }

    // @brief Converts PixelBuffer to VectorFieldBuffer.
    inline std::shared_ptr<VectorFieldBuffer> toVectorField(const std::shared_ptr<PixelBuffer>& pixelBuffer)
    {
        if (!pixelBuffer) return nullptr;
        const auto size = pixelBuffer->getSize();
        auto vectorField = std::make_shared<VectorFieldBuffer>(size);

        for (unsigned int y = 0; y < size.y; ++y) {
            for (unsigned int x = 0; x < size.x; ++x) {
                const sf::Color color = pixelBuffer->getPixel({x, y});

                const float vecX = (static_cast<float>(color.r) / 127.5f) - 1.0f; // [0, 255] -> [-1, 1]
                const float vecY = (static_cast<float>(color.g) / 127.5f) - 1.0f; // [0, 255] -> [-1, 1]

                vectorField->setVector({x, y}, {vecX, vecY});
            }
        }
        return vectorField;
    }

    // @brief Calculates the average brightness of GrayscaleBuffer.
    inline float toFloat(const std::shared_ptr<GrayscaleBuffer>& grayscaleBuffer)
    {
        if (!grayscaleBuffer || (grayscaleBuffer->getSize().x * grayscaleBuffer->getSize().y == 0)) {
            return 0.0f;
        }
        unsigned long long sum = 0;
        const auto size = grayscaleBuffer->getSize();
        const auto* data = grayscaleBuffer->getData();
        const size_t totalPixels = size.x * size.y;

        for (size_t i = 0; i < totalPixels; ++i) {
            sum += data[i];
        }

        return static_cast<float>(sum) / static_cast<float>(totalPixels) / 255.f;
    }

    inline sf::Color hsvToRgb(float h, const float s, const float v) {
        h = std::fmod(h, 1.0f);
        if (h < 0) h += 1.0f;

        const int i = static_cast<int>(h * 6);
        const float f = h * 6 - static_cast<float>(i);
        const float p = v * (1 - s);
        const float q = v * (1 - f * s);
        const float t = v * (1 - (1 - f) * s);

        switch (i % 6) {
            case 0: return {static_cast<uint8_t>(v * 255), static_cast<uint8_t>(t * 255), static_cast<uint8_t>(p * 255)};
            case 1: return {static_cast<uint8_t>(q * 255), static_cast<uint8_t>(v * 255), static_cast<uint8_t>(p * 255)};
            case 2: return {static_cast<uint8_t>(p * 255), static_cast<uint8_t>(v * 255), static_cast<uint8_t>(t * 255)};
            case 3: return {static_cast<uint8_t>(p * 255), static_cast<uint8_t>(q * 255), static_cast<uint8_t>(v * 255)};
            case 4: return {static_cast<uint8_t>(t * 255), static_cast<uint8_t>(p * 255), static_cast<uint8_t>(v * 255)};
            case 5: return {static_cast<uint8_t>(v * 255), static_cast<uint8_t>(p * 255), static_cast<uint8_t>(q * 255)};

            default: return sf::Color::Black;
        }
    }

} // namespace PGS::NodeGraph::Converters