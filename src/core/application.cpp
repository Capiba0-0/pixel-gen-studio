#include <PGS/core/application.h>     // Додати по-нормальному заголовки
#include <PGS/gui/imgui_setup.h> //
#include "imgui-SFML.h"          //
#include <stdexcept>             //

// --- Constructors ---
PGS::Application::Application()
	: Application(sf::Vector2u(sf::VideoMode::getDesktopMode().size.x / 2,
		sf::VideoMode::getDesktopMode().size.y / 2))
{ }

PGS::Application::Application(sf::Vector2u windowSize)
	: m_window{sf::VideoMode(windowSize), "PixelGen Studio"}
	, m_deltaClock{}
	, m_imguiIsInitialized{false}

	, m_minWindowSize{400, 300}
{
	if (!m_window.isOpen())
		throw std::runtime_error("FATAL ERROR: Failed to create SFML window.");

	m_window.setFramerateLimit(60);

	// ImGui-SGML init
	if (!ImGui::SFML::Init(m_window))
		throw std::runtime_error("FATAL ERROR: Failed to initialize ImGui-SFML. UI will not be available.");
	m_imguiIsInitialized = true;

	// ImGui Setup
	PGS::gui::applyImguiStyle();
	PGS::gui::setFonts();
}

// --- Destructor ---
PGS::Application::~Application()
{
	if (m_imguiIsInitialized)
		ImGui::SFML::Shutdown();
}


// --- Private methods ---
void PGS::Application::menuBar(float FramePaddingY)
{
	// Set Menu bar style
	float FramePaddingX = ImGui::GetStyle().FramePadding.x;
	ImVec2 newFramePadding{ FramePaddingX, FramePaddingY };

	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, newFramePadding);

	// Menu bar
	if (ImGui::BeginMainMenuBar()) {
		if (ImGui::BeginMenu("File")) {
			if (ImGui::MenuItem("Open", "Ctrl+O")) {}
			if (ImGui::MenuItem("Save", "Ctrl+S")) {}
			if (ImGui::MenuItem("Exit", "Escape")) {}

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

	// Delete Menu bar style
	ImGui::PopStyleVar();
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
				if (windowCurSize.x < m_minWindowSize.x || windowCurSize.y < m_minWindowSize.y)
					m_window.setSize(sf::Vector2u(std::max(windowCurSize.x, m_minWindowSize.x),
												  std::max(windowCurSize.y, m_minWindowSize.y)));
			}
		}
		
		ImGui::SFML::Update(m_window, m_deltaClock.restart());

		// GUI Items
		menuBar(12.0f);
		ImGui::ShowStyleEditor();

		m_window.clear(sf::Color(21, 21, 21, 255));
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