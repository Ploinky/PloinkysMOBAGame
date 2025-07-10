#pragma once

#include "vector2.h"

class Circle {
public:
	Circle() : position({ 0, 0 }), radius(0) {};
	Circle(Vector2 position, float radius) : position(position), radius(radius) {};

	Vector2 position;
	float radius;
};