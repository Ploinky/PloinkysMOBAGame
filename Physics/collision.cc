#include "collision.h"
#include <math.h>

namespace PMG::Physics {
	bool TestCollision(Ray ray, Sphere sphere) {
        float xd = ray.direction.x;
        float yd = ray.direction.y;
        float zd = ray.direction.z;

        float xo = ray.origin.x;
        float yo = ray.origin.y;
        float zo = ray.origin.z;

        float a = sphere.origin.x;
        float b = sphere.origin.y;
        float c = sphere.origin.z;

        float r = sphere.radius;

        float A = (pow(xd, 2) + pow(yd, 2) + pow(zd, 2));
        float B = (2 * (xd * (xo - a) + yd * (yo - b) + zd * (zo - c)));
        float C = (pow((xo - a), 2) + pow((yo - b), 2) + pow((zo - c), 2) - pow(r, 2));

        float t = (-B - sqrt(pow(B, 2) - 4.0f * A * C)) / 2.0f * A;

        return t >= 0;
	}

    bool TestCollision(Sphere sphere_a, Sphere sphere_b) {
        double dist = (sphere_a.origin - sphere_b.origin).Length();
        
        return dist <= sphere_a.radius + sphere_b.radius;
    }

    bool TestCollision(Circle circle_a, Circle circle_b) {
        double dist = (circle_a.position - circle_b.position).Length();

        return dist <= circle_a.radius + circle_b.radius;
    }
}