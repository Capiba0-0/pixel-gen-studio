#include "PGS/core/managers/document_manager.h"

#include "PGS/core/config.h"
#include "PGS/generators/procedural_generator.h"

PGS::DocumentManager::DocumentManager()
    : m_pixelBuffer{ std::make_shared<PixelBuffer>(m_canvasConfig.getDefaultSize())}
    , m_canvasView{ m_pixelBuffer }
{
    m_pixelBuffer->clear(sf::Color::White);
    m_canvasView.markForUpdate(); // Just to be safe
}

void PGS::DocumentManager::createNewDocument(sf::Vector2u size, const sf::Color color)
{
    if (size.x == 0 || size.y == 0) return;

    const auto newPixelBuffer = std::make_shared<PixelBuffer>(size);
    newPixelBuffer->clear(color);

    m_pixelBuffer = newPixelBuffer;
    m_canvasView.setPixelBuffer(newPixelBuffer); // Reset PixelBuffer
}

void PGS::DocumentManager::applyGenerator(generators::ProceduralGeneratorBase& generator)
{
    if (!m_pixelBuffer) return;

    generator.generate(*m_pixelBuffer);

    m_canvasView.markForUpdate();
}


void PGS::DocumentManager::update(sf::Time deltaTime)
{
    m_canvasView.update();
}

void PGS::DocumentManager::render(sf::RenderTarget& target, const sf::FloatRect& bounds)
{
	m_canvasView.render(target, bounds, CANVAS_PADDING_FACTOR);
}


