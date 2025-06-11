#include "PGS/core/application.h"

// -- PGS Headers --
#include "PGS/gui/imgui_setup.h"
#include "PGS/gui/imgui_helpers.h"

// -- Libraries Headers --
#include "imgui.h"
#include "imgui-SFML.h"
#include <SFML/Window/Event.hpp>

// -- STD Headers --
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
	, m_canvas{ std::make_unique<PGS::Canvas>(m_canvasConfig.getDefaultSize()) }

	, m_icon{}
	, m_uiContext{}

	// Windows
	, m_newCanvasWindow{}
	, m_aboutWindow{m_icon}
{
	if (!m_window.isOpen())
		throw std::runtime_error("FATAL ERROR: Failed to create SFML window.");

	m_window.setFramerateLimit(60);

	// ImGui-SGML init
	if (!ImGui::SFML::Init(m_window))
		throw std::runtime_error("FATAL ERROR: Failed to initialize ImGui-SFML. UI will not be available.");
	m_imguiIsInitialized = true;

	// Initialize PGS icon
	if (!m_icon.loadFromFile("assets/icons/Icon.png"))
		throw std::runtime_error("Failed to load icon texture from assets/icons");
	m_icon.setSmooth(true);

	// ImGui Setup
	PGS::gui::applyImguiStyle();
	PGS::gui::setFonts();

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
void PGS::Application::menuBar()
{
	// Style config
	ImGuiStyle& style = ImGui::GetStyle();
	ImVec2 newFramePadding{ style.FramePadding.x, MENU_BAR_PADDING_Y };
	PGS::gui::pusherStyleVar framePadding(ImGuiStyleVar_FramePadding, newFramePadding);
	PGS::gui::pusherStyleColor button(ImGuiCol_Button, sf::Color{ 38, 38, 38, 255 });
	PGS::gui::pusherStyleColor buttonActive(ImGuiCol_ButtonActive, sf::Color{ 76, 76, 76, 255 });
	PGS::gui::pusherStyleColor buttonHovered(ImGuiCol_ButtonHovered, sf::Color{ 76, 76, 76, 255 });
	
	// Menu bar
	if (ImGui::BeginMainMenuBar()) {

		if (ImGui::ImageButton("About PixelGen Studio", m_icon, sf::Vector2f{ 18, 18 })) {
			m_aboutWindow.open();
		}
		
		if (ImGui::BeginMenu("File")) {
			if (ImGui::MenuItem("New", "Ctrl+N")) {
				m_newCanvasWindow.open();
			}
			if (ImGui::MenuItem("Open", "Ctrl+O")) {}
			if (ImGui::MenuItem("Save", "Ctrl+S")) {}
			if (ImGui::MenuItem("Exit", "Ctrl+Q")) {}

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Edit")) {
			if (ImGui::MenuItem("Undo", "Ctrl+Z")) {}
			if (ImGui::MenuItem("Redo", "Ctrl+Y")) {}

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Window")) {

			ImGui::EndMenu();
		}
	}
	ImGui::EndMainMenuBar();
}

void PGS::Application::createNewCanvas(sf::Vector2u size, sf::Color color)
{
	createNewCanvas(size.x, size.y, color);
}
void PGS::Application::createNewCanvas(unsigned int x, unsigned int y, sf::Color color)
{
	if (x == 0 || y == 0) return;

	m_canvas = std::make_unique<PGS::Canvas>(x, y);

	m_canvas->clear(color);
	m_canvas->updateTexture();
}

// --- Public methods ---
void PGS::Application::run()
{
	while(m_window.isOpen()) {
		// Event processing
		while(const auto event = m_window.pollEvent()) {
			ImGui::SFML::ProcessEvent(m_window, *event);

			if (event->is<sf::Event::Closed>()) {
				m_window.close();
			}

			if (event->is<sf::Event::Resized>()) {
				sf::Vector2u windowCurSize = m_window.getSize();

				// Reset View of m_window
				sf::FloatRect visibleArea{ sf::Vector2f(0, 0), sf::Vector2f(windowCurSize.x, windowCurSize.y) };
				m_window.setView(sf::View(visibleArea));

				// Min limit of m_window resizing
				if (windowCurSize.x < MIN_WINDOW_SIZE.x || windowCurSize.y < MIN_WINDOW_SIZE.y)
					m_window.setSize(sf::Vector2u(std::max(windowCurSize.x, MIN_WINDOW_SIZE.x),
												  std::max(windowCurSize.y, MIN_WINDOW_SIZE.y)));
			}
		}
		
		ImGui::SFML::Update(m_window, m_deltaClock.restart());

		// GUI Items
		menuBar();
		
		// Windows
		m_aboutWindow.renderContent(m_uiContext);

		m_newCanvasWindow.renderContent(m_uiContext);
		if (m_uiContext.newCanvasRequest)
			createNewCanvas(m_uiContext.newCanvasRequest->size, m_uiContext.newCanvasRequest->bgColor);
	
		ImGui::ShowStyleEditor();

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