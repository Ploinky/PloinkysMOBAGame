#pragma once

#include "calculation.h"

namespace PMG::Physics {
	class Vector4 {
	public:
		Vector4();
		Vector4(double x, double y, double z, double w);

		double x;
		double y;
		double z;
		double w;

		Vector4 operator+(const Vector4& other) const {
			return { x + other.x, y + other.y, z + other.z, w + other.w };
		}

		Vector4 operator-(const Vector4& other) const {
			return { x - other.x, y - other.y, z - other.z, w - other.w };
		}

		bool operator==(const Vector4& other) const {
			return CompareDouble(x, other.x)
				&& CompareDouble(y, other.y)
				&& CompareDouble(z, other.z)
				&& CompareDouble(w, other.w);
		}
	};
}