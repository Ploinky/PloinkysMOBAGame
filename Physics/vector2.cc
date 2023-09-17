#include "vector2.h"
#include "calculation.h"
#include <math.h>

namespace PMG::Physics {
	Vector2::Vector2() {
		this->x = 0;
		this->y = 0;
	}

	Vector2::Vector2(float x, float y) : x(x), y(y) {

	}

	Vector2 Vector2::operator+(const Vector2& other) const {
		return { x + other.x, y + other.y };
	}

	Vector2 Vector2::operator-(const Vector2& other) const {
		return { x - other.x, y - other.y };
	}

	bool Vector2::operator==(const Vector2& other) const {
		return CompareFloat(x, other.x) && CompareFloat(y, other.y);
	}

	float Vector2::Length() {
		return sqrtf(x * x + y * y);
	}
}