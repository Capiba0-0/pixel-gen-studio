#pragma once

#include "PGS/gui/widget.h"

#include "imgui.h"
#include <SFML/System/Vector2.hpp>

namespace PGS::gui
{
struct PusherStyleVar
{
	// --- Constructors ---
	PusherStyleVar(ImGuiStyleVar idx, const ImVec2& val)
	{
		ImGui::PushStyleVar(idx, val);
	}
	PusherStyleVar(ImGuiStyleVar idx, const sf::Vector2f& val)
		: PusherStyleVar(idx, ImVec2{ val.x, val.y })
	{ }

	PusherStyleVar(ImGuiStyleVar idx, float& val)
	{
		ImGui::PushStyleVar(idx, val);
	}

	// --- Destructor ---
	~PusherStyleVar()
	{
		ImGui::PopStyleVar();
	}

	// No-copyable
	PusherStyleVar(const PusherStyleVar&) = delete;
	PusherStyleVar& operator=(const PusherStyleVar&) = delete;
};

struct PusherStyleColor
{
	// --- Constructors ---
	PusherStyleColor(ImGuiCol idx, const ImVec4& val)
	{
		ImGui::PushStyleColor(idx, val);
	}
	PusherStyleColor(ImGuiCol idx, const sf::Color& val)
	{
		ImVec4 color = ImVec4(
			static_cast<float>(val.r) / 255.0f,
			static_cast<float>(val.g) / 255.0f,
			static_cast<float>(val.b) / 255.0f,
			static_cast<float>(val.a) / 255.0f
		);
		ImGui::PushStyleColor(idx, color);
	}

	PusherStyleColor(ImGuiCol idx, float& val)
	{
		ImGui::PushStyleColor(idx, val);
	}

	// --- Destructor ---
	~PusherStyleColor()
	{
		ImGui::PopStyleColor();
	}

	// No-copyable
	PusherStyleColor(const PusherStyleColor&) = delete;
	PusherStyleColor& operator=(const PusherStyleColor&) = delete;
};

} // namespace PGS::gui