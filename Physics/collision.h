#pragma once

#include "ray.h"
#include "sphere.h"

namespace PMG::Physics {
	bool TestCollision(Ray ray, Sphere sphere);
}