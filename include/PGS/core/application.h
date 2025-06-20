#pragma once

// -- PGS Headers --
#include "PGS/core/canvas.h"
#include "PGS/core/config.h"
#include "PGS/gui/ui_context.h"
#include "PGS/gui/ui_manager.h"

// -- Libraries Headers --
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/System/Vector2.hpp>

// -- STL Headers --
#include <memory>
#include <vector>

namespace PGS
{
class Application
{
private:
	sf::RenderWindow m_window;
	sf::Clock m_deltaClock;
	bool m_imguiIsInitialized;

	static constexpr sf::Vector2u MIN_WINDOW_SIZE = { 400, 300 };

	config::CanvasConfig m_canvasConfig;
	std::unique_ptr<Canvas> m_canvas;

	sf::Texture m_icon;

	gui::UIManager m_uiManager;

	std::vector<events::UIEvent> m_eventQueue;

	// --- Private Methods ---
	void createNewCanvas(sf::Vector2u size, sf::Color color = sf::Color::White);
	void createNewCanvas(unsigned int x, unsigned int y, sf::Color color = sf::Color::White);

	void processAppEvent(const events::UIEvent& uiEvent);
	void processSystemEvent(gui::UIContext& context);
public:
	// --- Constructors - Destructor ---
	Application();
	explicit Application(sf::Vector2u windowSize);
	~Application();

	// Non-Copyable
	Application(const Application&) = delete;
	Application& operator=(const Application&) = delete;

	// --- Methods ---
	void run();

	void undo();
	void redo();
	void exit();
};

} // namespace PGS