#include "PGS/core/application.h"

// -- PGS Headers --
#include "PGS/gui/imgui_setup.h"
#include "PGS/gui/widgets/menu_bar.h"

// -- Libraries Headers --
#include "imgui.h"
#include "imgui-SFML.h"
#include <SFML/Window/Event.hpp>

// -- STL Headers --
#include <algorithm>
#include <stdexcept>

// --- Constructors ---
PGS::Application::Application()
	: Application(sf::Vector2u(sf::VideoMode::getDesktopMode().size.x / 2,
		                       sf::VideoMode::getDesktopMode().size.y / 2))
{ }

PGS::Application::Application(sf::Vector2u windowSize)
	: m_window{sf::VideoMode(windowSize), "PixelGen Studio"}
	, m_deltaClock{}
	, m_imguiIsInitialized{false}

	, m_canvasConfig{}
	, m_canvas{ std::make_unique<Canvas>(m_canvasConfig.getDefaultSize()) }

	, m_icon{}

	, m_uiManager(m_icon)
{
	if (!m_window.isOpen())
		throw std::runtime_error("FATAL ERROR: Failed to create SFML window.");

	m_window.setFramerateLimit(60);

	// ImGui-SGML init
	if (!ImGui::SFML::Init(m_window))
		throw std::runtime_error("FATAL ERROR: Failed to initialize ImGui-SFML. UI will not be available.");
	m_imguiIsInitialized = true;

	// Initialize PGS icon
	if (!std::filesystem::exists("assets/icons/Icon.png"))
		throw std::runtime_error("Icon file does not exist.");

	if (!m_icon.loadFromFile("assets/icons/Icon.png"))
		throw std::runtime_error("Failed to load icon texture from assets/icons");

	m_icon.setSmooth(true);

	// Menu Bar create
	m_uiManager.createWidget(typeid(gui::MenuBar));

	// ImGui Setup
	gui::applyImguiStyle();
	gui::setFonts();

	// Canvas Setup
	m_canvas->clear(sf::Color::White);
	m_canvas->updateTexture();
}

// --- Destructor ---
PGS::Application::~Application()
{
	if (m_imguiIsInitialized)
		ImGui::SFML::Shutdown();
}


// --- Private methods ---
void PGS::Application::createNewCanvas(sf::Vector2u size, sf::Color color)
{
	createNewCanvas(size.x, size.y, color);
}
void PGS::Application::createNewCanvas(unsigned int x, unsigned int y, const sf::Color color)
{
	if (x == 0 || y == 0) return;

	m_canvas = std::make_unique<PGS::Canvas>(x, y);

	m_canvas->clear(color);
	m_canvas->updateTexture();
}

void PGS::Application::processContextEvents(gui::UIContext& uiContext)
{
	if (uiContext.newCanvasRequest)
		createNewCanvas(uiContext.newCanvasRequest->size, uiContext.newCanvasRequest->bgColor);
}

void PGS::Application::processEvents(gui::UIContext& context)
{
	while(const auto event = m_window.pollEvent()) {
		ImGui::SFML::ProcessEvent(m_window, *event);

		// UIManager
		m_uiManager.onEvent(*event, context);

		if (event->is<sf::Event::Closed>()) {
			m_window.close();
		}

		if (event->is<sf::Event::Resized>()) {
			sf::Vector2u windowCurSize = m_window.getSize();

			// Reset View of m_window
			sf::FloatRect visibleArea {sf::Vector2f(0, 0),
				sf::Vector2f(static_cast<float>(windowCurSize.x), static_cast<float>(windowCurSize.y)) };
			m_window.setView(sf::View(visibleArea));

			// Min limit of m_window resizing
			if (windowCurSize.x < MIN_WINDOW_SIZE.x || windowCurSize.y < MIN_WINDOW_SIZE.y)
				m_window.setSize(sf::Vector2u(std::max(windowCurSize.x, MIN_WINDOW_SIZE.x),
											  std::max(windowCurSize.y, MIN_WINDOW_SIZE.y)));
		}
	}
}


// --- Public methods ---
void PGS::Application::run()
{
	while(m_window.isOpen()) {
		const sf::Time delta = m_deltaClock.restart();
		gui::UIContext uiContext {.uiManager = m_uiManager};

		// Event processing
		// Note: It doesn't work properly. The events of the previous frame should be sent here.
		//       This is a temporary solution. !!! Events don't work for now. !!!
		// TODO: Remake event/context system.
		processEvents(uiContext);
		processContextEvents(uiContext);

		ImGui::SFML::Update(m_window, delta);
		
		// UIManager
		m_uiManager.update(delta);
		m_uiManager.render(uiContext);
	
		ImGui::ShowStyleEditor(); // TODO: Delete this string, this is for testing

		m_window.clear(sf::Color(21, 21, 21, 255));

		// Canvas
		const ImGuiViewport* viewport = ImGui::GetMainViewport();
		sf::FloatRect canvasBounds{ sf::Vector2f{ viewport->WorkPos.x,  viewport->WorkPos.y  },
									sf::Vector2f{ viewport->WorkSize.x, viewport->WorkSize.y } };
		m_canvas->render(m_window, canvasBounds, 1.5);

		ImGui::SFML::Render(m_window);

		m_window.display();
	}
}

void PGS::Application::undo()
{

}

void PGS::Application::redo()
{

}

void PGS::Application::exit()
{

}