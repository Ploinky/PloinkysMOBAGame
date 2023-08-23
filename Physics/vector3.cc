#include "vector3.h"

namespace PMG::Physics {
	Vector3::Vector3() {
		this->x = 0;
		this->y = 0;
		this->z = 0;
	}

	Vector3::Vector3(double x, double y, double z) : x(x), y(y), z(z) {

	}
}