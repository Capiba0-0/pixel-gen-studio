#include "PGS/core/managers/document_manager.h"

#include "PGS/core/config.h"
#include "PGS/gui/ui_context.h"

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

std::shared_ptr<PGS::PixelBuffer> PGS::DocumentManager::getPixelBuffer()
{
    return m_pixelBuffer;
}


void PGS::DocumentManager::update(sf::Time deltaTime, const Gui::UIContext& context)
{
    const auto buffer = context.evaluator.evaluateFinalOutput(m_pixelBuffer->getSize());

    if (buffer != nullptr)
    {
        m_pixelBuffer = buffer;
        m_canvasView.setPixelBuffer(m_pixelBuffer);
    }

    m_canvasView.update();
}

void PGS::DocumentManager::render(sf::RenderTarget& target, const sf::FloatRect& bounds)
{
	m_canvasView.render(target, bounds, CANVAS_PADDING_FACTOR);
}


