#pragma once

#include "PGS/core/config.h"
#include "PGS/gui/canvas.h"

#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/System/Time.hpp>

#include <memory>

namespace PGS
{
    class PixelBuffer;

namespace generators
{
    class GeneratorAlgorithm;
}

namespace Gui
{
    struct UIContext;
}

class DocumentManager
{
private:
    const Config::CanvasConfig m_canvasConfig;
    std::shared_ptr<PixelBuffer> m_pixelBuffer;
    Gui::Canvas m_canvasView;

    const float CANVAS_PADDING_FACTOR = 1.5f;

public:
    DocumentManager();
    ~DocumentManager() = default;

    void createNewDocument(sf::Vector2u size, sf::Color color = sf::Color::White);

    std::shared_ptr<PixelBuffer> getPixelBuffer();

    void update(sf::Time deltaTime, const Gui::UIContext& context);
    void render(sf::RenderTarget& target, const sf::FloatRect& bounds);
};

} // namespace PGS