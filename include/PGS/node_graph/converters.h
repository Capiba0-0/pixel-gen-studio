#pragma once

#include "PGS/core/buffers/pixel_buffer.h"
#include "PGS/core/buffers/grayscale_buffer.h"
#include "PGS/core/buffers/vector_field_buffer.h"

#include "PGS/node_graph/types.h"

#include <cmath>
#include <memory>
#include <algorithm>

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

    inline HSV rgbToHsv(const sf::Color& rgb) {
        HSV hsv{};

        float r = static_cast<float>(rgb.r) / 255.0f;
        float g = static_cast<float>(rgb.g) / 255.0f;
        float b = static_cast<float>(rgb.b) / 255.0f;

        const float cMax = std::max({r, g, b});
        const float cMin = std::min({r, g, b});
        const float delta = cMax - cMin;

        // Hue
        if (delta == 0) {
            hsv.h = 0;
        } else if (cMax == r) {
            hsv.h = std::fmod(((g - b) / delta), 6.0f);
        } else if (cMax == g) {
            hsv.h = (b - r) / delta + 2.0f;
        } else { // cMax == b
            hsv.h = (r - g) / delta + 4.0f;
        }
        hsv.h /= 6.0f;
        if (hsv.h < 0) {
            hsv.h += 1.0f;
        }

        hsv.s = (cMax == 0) ? 0 : (delta / cMax);

        hsv.v = cMax;

        return hsv;
    }


    inline sf::Color hsvToRgb(const HSV& hsv ) {
        float r = 0, g = 0, b = 0;
        const float h = hsv.h, s = hsv.s, v = hsv.v;

        const int i = static_cast<int>(h * 6);
        const float f = h * 6 - static_cast<float>(i);
        const float p = v * (1 - s);
        const float q = v * (1 - f * s);
        const float t = v * (1 - (1 - f) * s);

        switch (i % 6) {
            case 0: r = v, g = t, b = p; break;
            case 1: r = q, g = v, b = p; break;
            case 2: r = p, g = v, b = t; break;
            case 3: r = p, g = q, b = v; break;
            case 4: r = t, g = p, b = v; break;
            case 5: r = v, g = p, b = q; break;
            default: r = g = b = 0; break;
        }

        return {
            static_cast<uint8_t>(std::clamp(r, 0.0f, 1.0f) * 255.0f),
            static_cast<uint8_t>(std::clamp(g, 0.0f, 1.0f) * 255.0f),
            static_cast<uint8_t>(std::clamp(b, 0.0f, 1.0f) * 255.0f)
        };
    }

} // namespace PGS::NodeGraph::Converters