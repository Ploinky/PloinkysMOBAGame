#include "vector3.h"

#include "calculation.h"
#include <math.h>

namespace PMG::Physics {
	Vector3::Vector3() {
		this->x = 0;
		this->y = 0;
		this->z = 0;
	}

	Vector3::Vector3(double x, double y, double z) : x(x), y(y), z(z) {

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
		return CompareDouble(x, other.x)
			&& CompareDouble(y, other.y)
			&& CompareDouble(z, other.z);
	}

	Vector3 Vector3::operator*(double scale) const {
		return { x * scale, y * scale, z * scale };
	}

	float Vector3::Length() const {
		return sqrt(x * x + y * y + z * z);
	}

	Vector3 Vector3::Normalize() const {
		float l = Length();

		if (l == 0) {
			return { 0, 0, 0 };
		}

		return { x / l, y / l, z / l };
	}

	Vector3 Vector3::ScaleToLength(double length) const {
		Vector3 scaled = Normalize();

		return scaled * length;
	}
}