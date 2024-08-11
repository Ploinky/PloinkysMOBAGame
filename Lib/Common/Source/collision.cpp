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

        float A = (powf(xd, 2) + powf(yd, 2) + powf(zd, 2));
        float B = (2 * (xd * (xo - a) + yd * (yo - b) + zd * (zo - c)));
        float C = (powf((xo - a), 2) + powf((yo - b), 2) + powf((zo - c), 2) - powf(r, 2));

        float t = (-B - sqrtf(powf(B, 2) - 4.0f * A * C)) / 2.0f * A;

        return t >= 0;
	}

    bool TestCollision(Sphere sphere_a, Sphere sphere_b) {
        float dist = (sphere_a.origin - sphere_b.origin).Length();
        
        return dist <= sphere_a.radius + sphere_b.radius;
    }

    bool TestCollision(Circle circle_a, Circle circle_b) {
        float dist = (circle_a.position - circle_b.position).Length();

        return dist <= circle_a.radius + circle_b.radius;
    }

    Vector2 TestCollision(Line line, Circle circle) {
        float x1 = line.Start.x - circle.position.x;
        float x2 = line.End.x - circle.position.x;
        float y1 = line.Start.y - circle.position.y;
        float y2 = line.End.y - circle.position.y;

        float dx = x2 - x1;
        float dy = y2 - y1;

        float dr = sqrtf(dx * dx + dy * dy);
        float D = x1 * y2 - x2 * y1;

        float discriminant = circle.radius * circle.radius * dr * dr - D * D;

        if (discriminant < 0) {
            // No intersection
            return { 0, 0 };
        }
        else if (discriminant == 0) {
            // Tangent
            // intersections[0].x = (D * dy) / (dr * dr) + circle.center.x;
            // intersections[0].y = (-D * dx) / (dr * dr) + circle.center.y;
            return { 0, 0 };
        }
        else {
            // Two intersections
            float sqrtDiscriminant = sqrtf(discriminant);
            float x_factor = sqrtDiscriminant * dy / (dr * dr);
            float y_factor = sqrtDiscriminant * dx / (dr * dr);
            
            return { (D * dy + dx * (dy > 0 ? 1 : -1) * sqrtDiscriminant) / (dr * dr) + circle.position.x,
                (-D * dx + (float)fabs(dy) * sqrtDiscriminant) / (dr * dr) + circle.position.y };
        }

        /*

        float x1 = line.Start.x;
        float x2 = line.End.x;
        float y1 = line.Start.y;
        float y2 = line.End.y;

        float dx = x2 - x1;
        float dy = y2 - y1;

        float dr = sqrtf((dx * dx) + (dy * dy));

        float D = x1 * y2 - x2 * y1;

        float sign = dy > 0 ? 1 : -1;

        float x = (D * dy + sign * dx * sqrtf((circle.radius * circle.radius) * (dr * dr) - (D * D)));
        float x0 = (D * dy - sign * dx * sqrtf((circle.radius * circle.radius) * (dr * dr) - (D * D)));
        float y = (-D * dx + fabs(dy) * sqrtf((circle.radius * circle.radius) * (dr * dr) - (D * D)));
        float y0 = (-D * dx - fabs(dy) * sqrtf((circle.radius * circle.radius) * (dr * dr) - (D * D)));

        float wtf = (circle.radius * circle.radius) * (dr * dr) - (D * D);

        if (wtf < 0) {
            // no intersection
            OutputDebugStringA("NO INTERSECT\n");
        }
        else if (wtf == 0) {
            // tangent
            OutputDebugStringA("TANGENT\n");
        }
        else {
            // intersection
            OutputDebugStringA("INTERSECT\n");
        }

        return 0;
        */

    }
}