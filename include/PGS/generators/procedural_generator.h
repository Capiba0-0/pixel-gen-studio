#pragma once

#include "PGS/gui/canvas.h"

namespace PGS::generators
{
class ProceduralGeneratorBase
{
public:
	// No-copyable
	ProceduralGeneratorBase(const ProceduralGeneratorBase&) = delete;
	ProceduralGeneratorBase& operator=(const ProceduralGeneratorBase&) = delete;

	virtual ~ProceduralGeneratorBase() = default;

	[[nodiscard]] virtual const char* getName() const = 0;

	virtual void renderUI() = 0;

	virtual void generate(PixelBuffer& pixelBuffer) = 0;

protected:
	ProceduralGeneratorBase() = default;
	ProceduralGeneratorBase(ProceduralGeneratorBase&&) = default;
	ProceduralGeneratorBase& operator=(ProceduralGeneratorBase&&) = default;
};

} // namespace PGS::generators