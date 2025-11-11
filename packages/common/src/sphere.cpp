#include "sphere.h"

Sphere::Sphere() {
	this->origin = Vector3(0.0f, 0.0f, 0.0f);
	this->radius = 1.0f;
}

Sphere::Sphere(Vector3 origin, float radius) : origin(origin), radius(radius) {

}