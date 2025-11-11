#pragma once

class Vector3 {
public:
	Vector3();
	Vector3(float x, float y, float z);

	float x;
	float y;
	float z;

	Vector3 operator+(const Vector3& other) const;
	Vector3 operator-(const Vector3& other) const;
	float operator*(const Vector3& other) const;
	Vector3 operator*(float scale) const;
	bool operator==(const Vector3& other) const;
	float Length() const;
	Vector3 Normalize() const;
	Vector3 ScaleToLength(float length) const;

	static Vector3 ZERO;
};