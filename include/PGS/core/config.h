#pragma once

#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/Color.hpp>

#include <vector>

namespace PGS::Config
{

struct CanvasConfig
{
	const std::vector<sf::Vector2u> sizeOptions = {
		{32, 32},
		{64, 64},
		{128, 128},
		{256, 256}
	};

	const int DEFAULT_SIZE_INDEX = 0;

	const sf::Color COLOR = sf::Color::White;

	[[nodiscard]] sf::Vector2u getDefaultSize() const
	{
		if (DEFAULT_SIZE_INDEX >= 0 && DEFAULT_SIZE_INDEX < sizeOptions.size())
			return sizeOptions[DEFAULT_SIZE_INDEX];

		return sf::Vector2u{ 32, 32 };
	}
};


} // namespace PGS::config