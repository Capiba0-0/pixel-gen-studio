#include "PGS/gui/ui_manager.h"

// -- PGS Headers --
#include "PGS/gui/ui_events.h"
#include "PGS/gui/widget.h"
#include "PGS/gui/widgets/about_window.h"
#include "PGS/gui/widgets/menu_bar.h"
#include "PGS/gui/widgets/new_canvas_window.h"
#include "PGS/gui/types.h"

// -- STL Headers --
#include <algorithm>

// --- Constructor ---
PGS::gui::UIManager::UIManager(sf::Texture& icon) // TODO: Normal Resource manager
{
    registerWidgetType<AboutWindow>(icon);
    registerWidgetType<MenuBar>(icon);
    registerWidgetType<NewCanvasWindow>();
}

// --- Methods ---
void PGS::gui::UIManager::render(UIContext& context)
{
    for (auto it : m_renderStack)
        m_widgets[it]->renderContent(context);
}

void PGS::gui::UIManager::onEvent(const sf::Event& event, UIContext& context)
{
    if (m_renderStack.empty())
        return;

    if (m_modalWidgetID.has_value())
    {
        if (const auto it = m_widgets.find(m_modalWidgetID.value()); it != m_widgets.end())
            it->second->onEvent(event);
    }

    else if (context.mouseButtonClicked.has_value())
    {
        const auto it = m_widgets.find(context.mouseButtonClicked->targetWidgetID);

        if (it == m_widgets.end())
            return;

        it->second->onEvent(event);

        switch (context.mouseButtonClicked->button)
        {
        case PGS::events::MouseButton::Left:
            requestFocus(context.mouseButtonClicked->targetWidgetID);
            break;
            // case PGS::events::MouseButton::Right:
            // case PGS::events::MouseButton::Middle:

        default:
            break;
        }
    }

    else if (const auto it = m_widgets.find(m_renderStack.back()); it != m_widgets.end())
        it->second->onEvent(event);
}

void PGS::gui::UIManager::update(sf::Time deltaTime)
{
    if (m_renderStack.empty())
        return;

    for (const auto& [id, widget] : m_widgets)
    {
        if (widget->isVisible())
            widget->update(deltaTime);
    }
}

// --- Private Methods ---
PGS::gui::WidgetID PGS::gui::UIManager::generateNextID()
{
    return s_nextWidgetID++;
}

// --- WidgetManager Overrides ---
PGS::gui::WidgetID PGS::gui::UIManager::createWidget(const std::type_index& typeIndex)
{
    const auto factory = m_factories.find(typeIndex);
    if (factory == m_factories.end())
        return INVALID_WIDGET_ID;

    std::unique_ptr<Widget> newWidget = factory->second();
    WidgetID newId = generateNextID();
    newWidget->setID(newId);

    m_widgets[newId] = std::move(newWidget);
    m_renderStack.push_back(newId);

    return newId;
}

void PGS::gui::UIManager::closeWidget(WidgetID id)
{
    if (m_widgets.find(id) == m_widgets.end())
        return;

    if (m_modalWidgetID == id)
        m_modalWidgetID.reset();

    m_widgets.erase(id);
    m_renderStack.erase(std::remove(m_renderStack.begin(), m_renderStack.end(), id), m_renderStack.end());
}

void PGS::gui::UIManager::requestFocus(WidgetID id)
{
    if (m_widgets.find(id) == m_widgets.end())
        return;

    m_renderStack.erase(std::remove(m_renderStack.begin(), m_renderStack.end(), id), m_renderStack.end());
    m_renderStack.push_back(id);
}

void PGS::gui::UIManager::requestModal(WidgetID id)
{
    if (m_widgets.find(id) == m_widgets.end())
        return;

    m_modalWidgetID = id;
}