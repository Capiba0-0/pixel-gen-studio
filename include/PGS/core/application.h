#pragma once

#include <SFML/Graphics.hpp> //
#include <SFML/System.hpp>   // Перевірити і додати заголовки по-нормальному
#include <SFML/Window.hpp>   //
#include "imgui.h"           //

namespace PGS
{
class Application
{
private:
	sf::RenderWindow m_window;
	sf::Clock m_deltaClock;
	bool m_imguiIsInitialized;

	const sf::Vector2u m_minWindowSize;

	// --- Private Methods ---
	void menuBar(float FramePaddingY);
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
