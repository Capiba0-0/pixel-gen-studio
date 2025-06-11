#pragma once

#include "imgui.h"
#include <SFML/System/Vector2.hpp>

namespace PGS::gui
{
struct pusherStyleVar
{
	// --- Constructors ---
	pusherStyleVar(ImGuiStyleVar idx, const ImVec2& val)
	{
		ImGui::PushStyleVar(idx, val);
	}
	pusherStyleVar(ImGuiStyleVar idx, const sf::Vector2f& val)
		: pusherStyleVar(idx, ImVec2{ val.x, val.y })
	{ }

	pusherStyleVar(ImGuiStyleVar idx, float& val)
	{
		ImGui::PushStyleVar(idx, val);
	}

	// --- Destructor ---
	~pusherStyleVar()
	{
		ImGui::PopStyleVar();
	}

	// No-copyable
	pusherStyleVar(const pusherStyleVar&) = delete;
	pusherStyleVar& operator=(const pusherStyleVar&) = delete;
};

struct pusherStyleColor
{
	// --- Constructors ---
	pusherStyleColor(ImGuiCol idx, const ImVec4& val)
	{
		ImGui::PushStyleColor(idx, val);
	}
	pusherStyleColor(ImGuiCol idx, const sf::Color& val)
	{
		ImVec4 color = ImVec4(
			static_cast<float>(val.r) / 255.0f,
			static_cast<float>(val.g) / 255.0f,
			static_cast<float>(val.b) / 255.0f,
			static_cast<float>(val.a) / 255.0f
		);
		ImGui::PushStyleColor(idx, color);
	}

	pusherStyleColor(ImGuiCol idx, float& val)
	{
		ImGui::PushStyleColor(idx, val);
	}

	// --- Destructor ---
	~pusherStyleColor()
	{
		ImGui::PopStyleColor();
	}

	// No-copyable
	pusherStyleColor(const pusherStyleColor&) = delete;
	pusherStyleColor& operator=(const pusherStyleColor&) = delete;
};

} // namespace PGS::gui