#pragma once

#include "vector3.h"

namespace PMG::Physics {
	class Sphere {
	public:
		Sphere();
		Sphere(Vector3 origin, double radius);

		Vector3 origin;
		double radius;
	};
}