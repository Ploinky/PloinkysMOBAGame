#include "sphere.h"

namespace PMG::Physics {
	Sphere::Sphere() {
		this->origin = Vector3(0.0, 0.0, 0.0);
		this->radius = 1.0;
	}

	Sphere::Sphere(Vector3 origin, double radius) : origin(origin), radius(radius) {

	}
}