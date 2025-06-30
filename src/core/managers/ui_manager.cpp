#include "PGS/core/managers/ui_manager.h"

// -- PGS Headers --
#include "PGS/gui/widget.h"
#include "PGS/gui/widgets/about_window.h"
#include "PGS/gui/widgets/menu_bar.h"
#include "PGS/gui/widgets/new_canvas_window.h"
#include "PGS/core/types.h"

// -- STL Headers --
#include <algorithm>

// --- Constructor ---
PGS::Gui::UIManager::UIManager(sf::Texture& icon) // TODO: Normal Resource manager
{
    registerWidgetType<AboutWindow>(icon);
    registerWidgetType<MenuBar>(icon);
    registerWidgetType<NewCanvasWindow>();
}

// --- Methods ---
void PGS::Gui::UIManager::render(UIContext& context)
{
    for (auto id : m_renderStack)
    {
        if (auto it = m_widgets.find(id); it != m_widgets.end())
            it->second->renderContent(context);
    }
}

void PGS::Gui::UIManager::onEvent(const sf::Event& event)
{
    if (m_renderStack.empty())
        return;

    if (m_modalWidgetID.has_value())
    {
        if (const auto it = m_widgets.find(m_modalWidgetID.value()); it != m_widgets.end())
            it->second->onEvent(event);
        return;
    }

    if (const auto it = m_widgets.find(m_renderStack.back()); it != m_widgets.end())
        it->second->onEvent(event);
}

void PGS::Gui::UIManager::update(const sf::Time deltaTime)
{
    if (m_renderStack.empty())
        return;

    for (auto id : m_renderStack)
    {
        if (auto it = m_widgets.find(id); it != m_widgets.end())
            it->second->update(deltaTime);
    }
}

PGS::Gui::WidgetID PGS::Gui::UIManager::widgetToId(Widget* widget)
{
    if (const auto it = m_mapWidgetToId.find(widget); it != m_mapWidgetToId.end()) {
        return it->second;
    }

    return INVALID_WIDGET_ID;
}


// --- Private Methods ---
PGS::Gui::WidgetID PGS::Gui::UIManager::generateNextID()
{
    return s_nextWidgetID++;
}

// --- WidgetManager Overrides ---
PGS::Gui::WidgetID PGS::Gui::UIManager::createWidget(const std::type_index& typeIndex)
{
    const auto factory = m_widgetFactories.find(typeIndex);
    if (factory == m_widgetFactories.end())
        return INVALID_WIDGET_ID;

    std::unique_ptr<Widget> newWidget = factory->second();
    const WidgetID newId = generateNextID();

    m_mapWidgetToId[newWidget.get()] = newId;
    m_widgets[newId] = std::move(newWidget);
    m_renderStack.push_back(newId);

    return newId;
}

void PGS::Gui::UIManager::closeWidget(const WidgetID id)
{
    if (m_widgets.find(id) == m_widgets.end())
        return;

    if (m_modalWidgetID == id)
        m_modalWidgetID.reset();

    Widget* ptr = m_widgets.at(id).get();

    m_renderStack.erase(std::remove(m_renderStack.begin(), m_renderStack.end(), id), m_renderStack.end());
    m_mapWidgetToId.erase(ptr);
    m_widgets.erase(id);
}

void PGS::Gui::UIManager::requestFocus(WidgetID id)
{
    if (m_widgets.find(id) == m_widgets.end())
        return;

    m_renderStack.erase(std::remove(m_renderStack.begin(), m_renderStack.end(), id), m_renderStack.end());
    m_renderStack.push_back(id);
}

void PGS::Gui::UIManager::requestModal(WidgetID id)
{
    if (m_widgets.find(id) == m_widgets.end())
        return;

    m_modalWidgetID = id;
}