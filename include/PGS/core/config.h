#pragma once

#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/Color.hpp>

#include <vector>

namespace PGS::config
{

struct CanvasConfig
{
	const std::vector<sf::Vector2u> sizeOptions = {
		{32, 32},
		{64, 64},
		{128, 128},
		{256, 256}
	};

	const int defaultSizeIndex = 0;

	const sf::Color color = sf::Color::White;

	[[nodiscard]] sf::Vector2u getDefaultSize() const
	{
		if (defaultSizeIndex >= 0 && defaultSizeIndex < sizeOptions.size())
			return sizeOptions[defaultSizeIndex];

		return sf::Vector2u{ 32, 32 };
	}
};


} // namespace PGS::config