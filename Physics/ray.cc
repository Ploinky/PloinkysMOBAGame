#include "ray.h"

namespace PMG::Physics {
	Ray::Ray() {
		origin = Vector3(0.0, 0.0, 0.0);
		direction = Vector3(0.0, 0.0, 0.0);
	}

	Ray::Ray(Vector3 origin, Vector3 direction) : origin(origin), direction(direction) {

	}
}