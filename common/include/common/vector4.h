#pragma once

#include "calculation.h"

struct mat_s {
    // m[row][column]
    float m[4][4] = { 0 };
};

class Vector4 {
public:
	Vector4();
	Vector4(float x, float y, float z, float w);

	float x;
	float y;
	float z;
	float w;

	Vector4 operator+(const Vector4& other) const {
		return { x + other.x, y + other.y, z + other.z, w + other.w };
	}

	Vector4 operator-(const Vector4& other) const {
		return { x - other.x, y - other.y, z - other.z, w - other.w };
	}

	bool operator==(const Vector4& other) const {
		return CompareFloat(x, other.x)
			&& CompareFloat(y, other.y)
			&& CompareFloat(z, other.z)
			&& CompareFloat(w, other.w);
	}
};