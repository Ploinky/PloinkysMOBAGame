#include "vector3.h"

#include "calculation.h"
#include <math.h>

Vector3 Vector3::ZERO = {0, 0, 0};

Vector3::Vector3() {
	this->x = 0;
	this->y = 0;
	this->z = 0;
}

Vector3::Vector3(float x, float y, float z) : x(x), y(y), z(z) {

}

Vector3 Vector3::operator+(const Vector3& other) const {
	return { x + other.x, y + other.y, z + other.z };
}

Vector3 Vector3::operator-(const Vector3& other) const {
	return { x - other.x, y - other.y, z - other.z };
}

float Vector3::operator*(const Vector3& other) const {
	return x * other.x + y * other.y + z * other.z;
}
bool Vector3::operator==(const Vector3& other) const {
	return CompareFloat(x, other.x)
		&& CompareFloat(y, other.y)
		&& CompareFloat(z, other.z);
}

Vector3 Vector3::operator*(float scale) const {
	return { x * scale, y * scale, z * scale };
}

float Vector3::Length() const {
	return sqrtf(x * x + y * y + z * z);
}

Vector3 Vector3::Normalize() const {
	float l = Length();

	if (l == 0) {
		return { 0, 0, 0 };
	}

	return { x / l, y / l, z / l };
}

Vector3 Vector3::ScaleToLength(float length) const {
	Vector3 scaled = Normalize();

	return scaled * length;
}