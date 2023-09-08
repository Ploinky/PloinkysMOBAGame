#pragma once

namespace PMG::Physics {
	class Vector2 {
	public:
		Vector2();
		Vector2(double x, double y);

		double x;
		double y;

		Vector2 operator+(const Vector2& other) const;

		Vector2 operator-(const Vector2& other) const;

		bool operator==(const Vector2& other) const;

		double Length();
	};
}