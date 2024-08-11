#pragma once

#include "vector3.h"

namespace PMG::Physics {
	class Ray {
	public:
		Ray();
		Ray(Vector3 origin, Vector3 direction);

		Vector3 origin;
		Vector3 direction;
	};
}