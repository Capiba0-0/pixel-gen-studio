#pragma once

// -- PGS Headers --
#include "PGS/core/canvas.h"
#include "PGS/core/config.h"
#include "PGS/gui/ui_context.h"
#include "PGS/gui/windows/about_window.h"
#include "PGS/gui/windows/new_canvas_window.h"

// -- Libraries Headers --
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/System/Vector2.hpp>

// -- STD Headers --
#include <memory>

namespace PGS
{
class Application
{
private:
	sf::RenderWindow m_window;
	sf::Clock m_deltaClock;
	bool m_imguiIsInitialized;

	inline static const sf::Vector2u MIN_WINDOW_SIZE = { 400, 300 };
	static constexpr float MENU_BAR_PADDING_Y = 12.0f;

	PGS::config::CanvasConfig m_canvasConfig;
	std::unique_ptr<PGS::Canvas> m_canvas;

	sf::Texture m_icon;
	PGS::gui::UIContext m_uiContext;

	// Windows
	PGS::gui::NewCanvasWindow m_newCanvasWindow;
	PGS::gui::AboutWindow m_aboutWindow;

	// --- Private Methods ---
	void menuBar();

	void createNewCanvas(sf::Vector2u size, sf::Color color = sf::Color::White);
	void createNewCanvas(unsigned int x, unsigned int y, sf::Color color = sf::Color::White);
public:
	// --- Constructors - Destructor ---
	Application();
	Application(sf::Vector2u windowSize);
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