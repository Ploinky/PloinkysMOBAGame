#pragma once

#include "ray.h"
#include "sphere.h"
#include "circle.h"
#include "Line.h"

namespace PMG::Physics {
	bool TestCollision(Ray ray, Sphere sphere);
	bool TestCollision(Sphere sphere_a, Sphere sphere_b);
	bool TestCollision(Circle circle_a, Circle circle_b);
	Vector2 TestCollision(Line line, Circle circle);
}