#pragma once

// -- PGS Headers --
#include "PGS/gui/canvas.h"
#include "PGS/gui/ui_context.h"
#include "PGS/core/managers/ui_manager.h"

// -- Libraries Headers --
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/System/Vector2.hpp>

// -- STL Headers --
#include <vector>

#include "managers/document_manager.h"

namespace PGS
{
class Application
{
private:
	sf::RenderWindow m_window;
	sf::Clock m_deltaClock;
	bool m_imguiIsInitialized; // TODO: Make RAII-wrapper for ImGui initialization

	static constexpr sf::Vector2u MIN_WINDOW_SIZE = { 400, 300 };

	sf::Texture m_icon;

	DocumentManager m_documentManager;
	Gui::UIManager m_uiManager;

	std::vector<Events::UIEvent> m_eventQueue;

	// --- Private Methods ---

	void processAppEvent(const Events::UIEvent& uiEvent);
	void processSystemEvent();
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