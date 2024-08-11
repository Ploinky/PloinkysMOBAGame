#pragma once

#include "vector2.h"

namespace PMG::Physics {
	bool CompareFloat(float a, float b);
	float CalculateAngle(Physics::Vector2 from, Physics::Vector2 to);
	float ToRadians(float degrees);
	float ToDegrees(float radians);
}