#pragma once

// -- PGS Headers --
#include "PGS/gui/ui_context.h"
#include "PGS/gui/widget.h"
#include "PGS/gui/ui_manager_interface.h"
#include "PGS/gui/types.h"

// -- Libraries Headers --
#include <SFML/Window/Event.hpp>
#include <SFML/System/Time.hpp>
#include <SFML/Graphics/Texture.hpp>

// -- STL Headers --
#include <memory>
#include <unordered_map>
#include <vector>
#include <typeindex>
#include <tuple>
#include <functional>

namespace PGS::gui
{

class UIManager : public UIManagerInterface
{
private:
    using WidgetFactory = std::function<std::unique_ptr<Widget>()>;
    std::unordered_map<std::type_index, WidgetFactory> m_factories;

    std::unordered_map<WidgetID, std::unique_ptr<Widget>> m_widgets;

    std::vector<WidgetID> m_renderStack;

    inline static WidgetID s_nextWidgetID = 1; // 0 is invalid id
    std::optional<WidgetID> m_modalWidgetID;

    // --- Private Methods ---
    [[nodiscard]] WidgetID generateNextID();
public:
    // --- Constructors ---
    // No copyable
    UIManager(const UIManager&) = delete;
    UIManager& operator=(const UIManager&) = delete;

    explicit UIManager(sf::Texture& icon);

    // --- Methods ---
    void render(UIContext& context);

    void onEvent(const sf::Event& event, UIContext& context);
    void update(sf::Time deltaTime);

    template<typename T, typename... Args>
    void registerWidgetType(Args&&... args)
    {
        static_assert(std::is_base_of_v<Widget, T>, "Type must be derived from Widget.");

        std::type_index typeIndex = typeid(T);

        auto args_tuple = std::forward_as_tuple(std::forward<Args>(args)...);

        m_factories[typeIndex] = [args_tuple]()
        {
            return std::apply(
                [](auto&&... args) { return std::make_unique<T>(std::forward<decltype(args)>(args)...); },
                args_tuple
            );
        };
    }

    // --- WidgetManager Overrides ---
    WidgetID createWidget(const std::type_index& typeIndex) override;
    void closeWidget(WidgetID id) override;

    void requestFocus(WidgetID id) override;
    void requestModal(WidgetID id) override;
};

} // namespace PGS::gui