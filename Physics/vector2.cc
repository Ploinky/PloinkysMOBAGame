#include "vector2.h"
#include "calculation.h"
#include <math.h>

namespace PMG::Physics {
	Vector2::Vector2() {
		this->x = 0;
		this->y = 0;
	}

	Vector2::Vector2(double x, double y) : x(x), y(y) {

	}

	Vector2 Vector2::operator+(const Vector2& other) const {
		return { x + other.x, y + other.y };
	}

	Vector2 Vector2::operator-(const Vector2& other) const {
		return { x - other.x, y - other.y };
	}

	bool Vector2::operator==(const Vector2& other) const {
		return CompareDouble(x, other.x) && CompareDouble(y, other.y);
	}

	double Vector2::Length() {
		return sqrt(x * x + y * y);
	}
}