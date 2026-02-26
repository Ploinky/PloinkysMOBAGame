#include "common/vector2.h"
#include "common/calculation.h"
#include <math.h>

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

bool Vector2::operator!=(const Vector2& other) const {
	return !CompareFloat(x, other.x) || !CompareFloat(y, other.y);
}

Vector2 Vector2::operator*(float scale) const {
	return { x * scale, y * scale };
}

float Vector2::operator*(const Vector2& other) const {
	return x * other.x + y * other.y;
}

float Vector2::Length() const {
	return sqrtf(x * x + y * y);
}

Vector2 Vector2::Normalize() const {
	float length = Length();

	if (length == 0) {
		return { 0, 0 };
	}

	return { x / length, y / length };
}

Vector2 Vector2::ScaleToLength(float length) const {
	Vector2 scaled = Normalize();

	return scaled * length;
}