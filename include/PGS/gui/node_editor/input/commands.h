#pragma once

#include "PGS/gui/node_editor/input/command.h"

#include "imgui.h"

#include <typeindex>

namespace PGS::Gui
{
class DeleteSelectedCommand final : public Command
{
public:
    void execute(UIContext& context, NodeEditorState& state) override;
};


class AddNodeCommand final : public Command
{
    std::type_index m_type;
    ImVec2 m_position;
public:
    AddNodeCommand(const std::type_index type, const ImVec2 position)
        : m_type(type), m_position(position)
    {}
    void execute(UIContext& context, NodeEditorState& state) override;
};


class CreateLinkCommand final : public Command
{
    int m_startAttrId;
    int m_endAttrId;
public:
    CreateLinkCommand(const int startAttr, const int endAttr)
        : m_startAttrId(startAttr), m_endAttrId(endAttr)
    {}
    void execute(UIContext& context, NodeEditorState& state) override;
};


class DestroyLinkCommand final : public Command
{
    int m_linkId;
public:
    explicit DestroyLinkCommand(const int linkId)
        : m_linkId(linkId)
    {}
    void execute(UIContext& context, NodeEditorState& state) override;
};


class DropLinkCommand final : public Command
{
    int m_startAttrId;
public:
    explicit DropLinkCommand(const int startAttrId)
        : m_startAttrId(startAttrId)
    {}
    void execute(UIContext& context, NodeEditorState& state) override;
};
} // namespace PGS::Gui

