#pragma once


namespace PMG::Physics {
	class Vector3 {
	public:
		Vector3();
		Vector3(double x, double y, double z);

		double x;
		double y;
		double z;

        Vector3 operator+(const Vector3& other) const;
        Vector3 operator-(const Vector3& other) const;
        float operator*(const Vector3& other) const;
		Vector3 operator*(double scale) const;
        bool operator==(const Vector3& other) const;
        float Length() const;
		Vector3 Normalize() const;
		Vector3 ScaleToLength(double length) const;
	};
}