#pragma once

#include "SFML/System/Vector2.hpp"
#include "SFML/Graphics/Color.hpp"

#include <memory>
#include <optional>
#include <vector>

namespace PGS
{

// Start Declaration
class PixelBuffer;

namespace algorithm
{
	class GeneratorAlgorithm;
}

// End Declaration

namespace algorithm
{

enum class BlendMode
{
	Normal,
	Add,
	Multiply
	// Other
};

enum class DistortionMode
{
	XY,
	X_ONLY,
	Y_ONLY,
	ANGLE
};

class GeneratorAlgorithm
{
public:
	virtual ~GeneratorAlgorithm() = default;

	[[nodiscard]] virtual std::unique_ptr<PixelBuffer> generate(const sf::Vector2u& size) const;

	[[nodiscard]] virtual const char* getName() const = 0;

	void connectPositionDistortion(GeneratorID sourceId);
	void connectIntensityMask(GeneratorID sourceId);
	void connectColorModulation(GeneratorID sourceId);

	void addDependent(GeneratorID dependentId);
	void removeDependent(GeneratorID dependentId);

	void markDirty();

protected:
	GeneratorID m_id;

	std::optional<GeneratorID> m_positionDistortionSourceID;
	std::optional<GeneratorID> m_intensityMaskSourceID;
	std::optional<GeneratorID> m_colorSourceID;

	std::vector<GeneratorID> m_dependents;

	std::unique_ptr<PixelBuffer> m_cachedOutput;
	bool m_isDirty = true;

	// Distortion Settings
	float m_distortionStrength = 0; // 0+
	DistortionMode m_distortionMode = DistortionMode::XY;

	// Mask Settings
	float m_maskStrength = 0; // 0.0 - 1.0
	bool m_maskInvert = false;
	float m_maskInputMin = 0; // 0.0 - 1.0
	float m_maskInputMax = 0; // 0.0 - 1.0

	// Color Settings
	BlendMode m_colorMode = BlendMode::Normal;
	float m_colorInfluence = 0; // 0.0 - 1.0

	// -- Protected Methods --
	[[nodiscard]] virtual sf::Color generatePixel(const sf::Vector2u& pos, const sf::Color& color) const = 0;
};

} // namespace algorithms
} // namespace PGS