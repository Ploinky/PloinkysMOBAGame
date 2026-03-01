#include "common/collision.h"
#include <math.h>
#include <algorithm>

bool TestCollision(Ray ray, Capsule_t cap) {
    Vector3 vec3RayDirNorm = ray.direction.Normalize();
    Vector3 vec3LineSeg = cap.vec3End - cap.vec3Start;
    Vector3 w0 = ray.origin - cap.vec3Start;

    double a = vec3RayDirNorm * vec3RayDirNorm;
    double b = vec3RayDirNorm * vec3LineSeg;
    double c = vec3LineSeg * vec3LineSeg;
    double d = vec3RayDirNorm * w0;
    double e = vec3LineSeg * w0;

    double denom = a * c - b * b;

    double t_c = 0.0, u_c = 0.0;

    if (denom != 0.0) {
        t_c = (b * e - c * d) / denom;
        u_c = (a * e - b * d) / denom;
    }

    if (t_c < 0.0) {
        t_c = 0.0;
    }

    double u_c_clamped = std::clamp(u_c, 0.0, 1.0);

    if (denom != 0.0 && u_c != u_c_clamped) {
        u_c = u_c_clamped;
        t_c = (b * u_c + d) / a;
        if (t_c < 0.0) t_c = 0.0;
    }

    Vector3 closestPointRay = ray.origin + vec3RayDirNorm * t_c;
    Vector3 closestPointSeg = cap.vec3Start + vec3LineSeg * u_c_clamped;

    return (closestPointRay - closestPointSeg).Length() < cap.fRadius;
}

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

// with thanks to bobobobo & co.
// https://stackoverflow.com/a/1084899
Vector2 TestCollision(Line line, Circle circle) {
    Vector2 d = line.End - line.Start;
    Vector2 f = line.Start - circle.position;
    float r = circle.radius;
    float a = d * d;
    float b = 2 * (f * d);
    float c = (f * f) - r * r;

    float discriminant = b * b - 4 * a * c;
    if (discriminant < 0)
    {
        return { 0, 0 };
    }

    discriminant = sqrt(discriminant);

    float t1 = (-b - discriminant) / (2 * a);
    float t2 = (-b + discriminant) / (2 * a);

    if (t1 >= 0 && t1 <= 1)
    {
        return line.Start + (line.End - line.Start).ScaleToLength((line.End - line.Start).Length() * t1);
    } else if(t1 < 0 && t2 >= 0 && t2 <= 1) {
        return line.Start + (line.End - line.Start).ScaleToLength((line.End - line.Start).Length() * t1);
    }

    return { 0, 0 };
}

// with thanks to Paul Bourke
// https://web.archive.org/web/20060911055655/http://local.wasp.uwa.edu.au/%7Epbourke/geometry/lineline2d/
std::optional<Vector2> TestCollision(Line a, Line b) {
    float x1 = a.Start.x;
    float y1 = a.Start.y;
    float x2 = a.End.x;
    float y2 = a.End.y;
    float x3 = b.Start.x;
    float y3 = b.Start.y;
    float x4 = b.End.x;
    float y4 = b.End.y;

    float denominator = ((y4 - y3) * (x2 - x1) - (x4 - x3) * (y2 - y1));

    if(denominator == 0) {
        return std::nullopt;
    }

    
    float ua = ((x4 - x3) * (y1 - y3) - (y4 - y3) * (x1 - x3)) / denominator;
    
    
    float ub = ((x2 - x1) * (y1 - y3) - (y2 - y1) * (x1 - x3)) / denominator;
    if(ua > 1 || ua < 0 || ub > 1 || ub < 0) {
        return std::nullopt;
    }
    float x = x1 + ua * (x2 - x1);
    float y = y1 + ua * (y2 - y1);


    return std::make_optional<Vector2>({x, y});
}