#pragma once

// -- PGS Headers --
#include "PGS/gui/ui_context.h"
#include "PGS/gui/widget.h"
#include "PGS/core/managers/ui_manager_interface.h"

// -- Libraries Headers --
#include <SFML/Graphics/Texture.hpp>

// -- STL Headers --
#include <memory>
#include <optional>
#include <unordered_map>
#include <typeindex>
#include <functional>

namespace PGS::Gui
{

class UIManager final : public UIManagerInterface
{
private:
    using WidgetFactory = std::function<std::unique_ptr<Widget>()>;
    std::unordered_map<std::type_index, WidgetFactory> m_widgetFactories;

    std::unordered_map<WidgetID, std::unique_ptr<Widget>> m_widgets;
    std::vector<WidgetID> m_renderStack;
    std::unordered_map<Widget*, WidgetID> m_mapWidgetToId;

    inline static WidgetID s_nextWidgetID = 1; // 0 is invalid ID
    std::optional<WidgetID> m_modalWidgetID;

    // --- Private Methods ---
    [[nodiscard]] static WidgetID generateNextID();
public:
    // --- Constructors ---
    // No copyable
    UIManager(const UIManager&) = delete;
    UIManager& operator=(const UIManager&) = delete;

    explicit UIManager(sf::Texture& icon);

    // --- Methods ---
    void render(UIContext& context);

    void onEvent(const sf::Event& event);
    void update(sf::Time deltaTime);

    WidgetID widgetToId(Widget* widget);

    template<typename T, typename... Args>
    void registerWidgetType(Args&&... args)
    {
        static_assert(std::is_base_of_v<Widget, T>, "Type must be derived from PGS::Gui::Widget");

        const std::type_index typeIndex = typeid(T);

        auto argsTuple = std::forward_as_tuple(std::forward<Args>(args)...);

        m_widgetFactories[typeIndex] = [argsTuple]()
        {
            return std::apply(
                [](auto&&... args) { return std::make_unique<T>(std::forward<decltype(args)>(args)...); },
                argsTuple
            );
        };
    }

    // --- WidgetManager Overrides ---
    WidgetID createWidget(const std::type_index& typeIndex) override;
    WidgetID createWidget(const std::type_index& typeIndex, const std::function<void(Widget&)>& initializer) override;
    void closeWidget(WidgetID id) override;

    void requestFocus(WidgetID id) override;
    void requestModal(WidgetID id) override;
};

} // namespace PGS::gui