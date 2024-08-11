#pragma once

namespace PMG::Physics {
	class Vector2 {
	public:
		Vector2();
		Vector2(float x, float y);

		float x;
		float y;

		Vector2 operator+(const Vector2& other) const;

		Vector2 operator-(const Vector2& other) const;

		bool operator==(const Vector2& other) const;

		Vector2 operator*(float scale) const;

		float Length() const;

		Vector2 Normalize() const;
		Vector2 ScaleToLength(float length) const;
	};
}