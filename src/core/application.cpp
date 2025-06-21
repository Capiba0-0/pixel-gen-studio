#include "PGS/core/application.h"

// -- PGS Headers --
#include "PGS/gui/imgui_setup.h"
#include "PGS/gui/widgets/menu_bar.h"
#include "PGS/gui/ui_events.h"
#include "PGS/gui/ui_context.h"

// -- Libraries Headers --
#include "imgui.h"
#include "imgui-SFML.h"
#include <SFML/Window/Event.hpp>

// -- STL Headers --
#include <algorithm>
#include <stdexcept>
#include <type_traits>

// --- Constructors ---
PGS::Application::Application()
	: Application(sf::Vector2u(sf::VideoMode::getDesktopMode().size.x / 2,
		                       sf::VideoMode::getDesktopMode().size.y / 2))
{ }

PGS::Application::Application(sf::Vector2u windowSize)
	: m_window{sf::VideoMode(windowSize), "PixelGen Studio"}
	, m_imguiIsInitialized{false}

	, m_documentManager{}

	, m_uiManager(m_icon)
{
	if (!m_window.isOpen())
		throw std::runtime_error("FATAL ERROR: Failed to create SFML window.");
		// TODO: Make new error-class for Fatal Errors

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
	gui::applyImguiStyle();
	gui::setFonts();

	// Menu Bar create
	m_uiManager.createWidget(typeid(gui::MenuBar));
}

// --- Destructor ---
PGS::Application::~Application()
{
	if (m_imguiIsInitialized)
		ImGui::SFML::Shutdown();
}

// --- Private methods ---
void PGS::Application::processAppEvent(const events::UIEvent& uiEvent)
{
	std::visit([this](auto&& arg) {
		using T = std::decay_t<decltype(arg)>;

		if constexpr (std::is_same_v<T, events::NewCanvasRequest>)
		{
			m_documentManager.createNewDocument(arg.size, arg.bgColor);
		}

		else if constexpr (std::is_same_v<T, events::CloseWidget>)
		{
			m_uiManager.closeWidget(m_uiManager.widgetToId(arg.targetWidget));
		}
		else if constexpr (std::is_same_v<T, events::RequestFocus>)
		{
			m_uiManager.requestFocus(m_uiManager.widgetToId(arg.targetWidget));
		}
		else if constexpr (std::is_same_v<T, events::RequestModal>)
		{
			m_uiManager.requestModal(m_uiManager.widgetToId(arg.targetWidget));
		}

	}, uiEvent);
}

void PGS::Application::processSystemEvent()
{
	while(const auto event = m_window.pollEvent()) {
		ImGui::SFML::ProcessEvent(m_window, *event);

		// UIManager
		m_uiManager.onEvent(*event);

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
		// -- UIEvent processing --
		for (const auto& event : m_eventQueue)
			processAppEvent(event);
		m_eventQueue.clear();

		// -- New Frame parameters --
		const sf::Time deltaTime = m_deltaClock.restart();

		const gui::EventEmitter emitter = [&](const events::UIEvent& uiEvent)
		{
			m_eventQueue.push_back(uiEvent);
		};

		gui::UIContext context
		{
			.emit = emitter,
			.deltaTime = deltaTime,
			.uiManager = m_uiManager
		};

		// Canvas Parameters
		const ImGuiViewport* viewport = ImGui::GetMainViewport();
		const sf::FloatRect canvasBounds{ sf::Vector2f{ viewport->WorkPos.x,  viewport->WorkPos.y  },
									sf::Vector2f{ viewport->WorkSize.x, viewport->WorkSize.y } };

		// -- System Event processing --
		processSystemEvent();

		// -- Update --
		ImGui::SFML::Update(m_window, deltaTime);
		m_uiManager.update(deltaTime);
		m_documentManager.update(deltaTime);

		// -- Render --
		m_window.clear(sf::Color(21, 21, 21, 255));

		ImGui::ShowStyleEditor(); // TODO: Delete this string, this is for testing

		m_uiManager.render(context);
		m_documentManager.render(m_window, canvasBounds);
		ImGui::SFML::Render(m_window);

		// -- Display --
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