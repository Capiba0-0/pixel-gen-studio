#pragma once

#include "PGS/core/types.h"

#include <functional>
#include <typeindex>

namespace PGS::Gui
{

class UIManagerInterface
{
public:
    virtual ~UIManagerInterface() = default;

    virtual WidgetID createWidget(const std::type_index& typeIndex) = 0;
    virtual WidgetID createWidget(const std::type_index& typeIndex, const std::function<void(Widget&)>& initializer) = 0;
    virtual void closeWidget(WidgetID id) = 0;

    virtual void requestFocus(WidgetID id) = 0;
    virtual void requestModal(WidgetID id) = 0;
};

} // namespace PGS::gui
