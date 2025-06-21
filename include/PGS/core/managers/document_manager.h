#pragma once

#include "PGS/gui/canvas.h"

#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/System/Time.hpp>

#include <memory>

#include "PGS/core/config.h"

namespace PGS
{

class PixelBuffer;

namespace generators {
    class ProceduralGeneratorBase;
}

class DocumentManager
{
private:
    config::CanvasConfig m_canvasConfig;
    std::shared_ptr<PixelBuffer> m_pixelBuffer;
    gui::Canvas m_canvasView;

    const float CANVAS_PADDING_FACTOR = 1.5f;

public:
    DocumentManager();
    ~DocumentManager() = default;

    void createNewDocument(sf::Vector2u size, sf::Color color = sf::Color::White);

    void applyGenerator(generators::ProceduralGeneratorBase& generator);

    void update(sf::Time deltaTime);
    void render(sf::RenderTarget& target, const sf::FloatRect& bounds);
};

} // namespace PGS