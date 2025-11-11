#pragma once

#include "vector3.h"

class Sphere {
public:
	Sphere();
	Sphere(Vector3 origin, float radius);

	Vector3 origin;
	float radius;
};