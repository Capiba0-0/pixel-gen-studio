#pragma once
#include "PGS/core/canvas.h"

namespace PGS::generators
{
class ProceduralGeneratorBase
{
public:
	virtual ~ProceduralGeneratorBase() = default;

	virtual const char* getName() const = 0;

	virtual void renderUI() = 0;

	virtual void generate(PGS::Canvas& canvas) = 0;

protected:
	// No-copyable
	ProceduralGeneratorBase(const ProceduralGeneratorBase&) = delete;
	ProceduralGeneratorBase& operator=(const ProceduralGeneratorBase&) = delete;

	ProceduralGeneratorBase() = default;
	ProceduralGeneratorBase(ProceduralGeneratorBase&&) = default;
	ProceduralGeneratorBase& operator=(ProceduralGeneratorBase&&) = default;
};

} // namespace PGS::generators