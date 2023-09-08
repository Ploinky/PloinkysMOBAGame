#pragma once

#include "vector2.h"

namespace PMG::Physics {
	class Circle {
	public:
		Circle() : position({ 0, 0 }), radius(0) {};
		Circle(Vector2 position, double radius) : position(position), radius(radius) {};

		Vector2 position;
		double radius;
	};
}