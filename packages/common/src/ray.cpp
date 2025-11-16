#include "ray.h"

Ray::Ray() {
	origin = Vector3(0.0f, 0.0f, 0.0f);
	direction = Vector3(0.0f, 0.0f, 0.0f);
}

Ray::Ray(Vector3 origin, Vector3 direction) : origin(origin), direction(direction) {

}