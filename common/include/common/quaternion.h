#pragma once

#include <cmath>

class Quaternion {
public:
	float x;
	float y;
	float z;
	float w;
};

inline Quaternion Slerp(const Quaternion& q1, const Quaternion& q2, float t)
{
    // Compute the cosine of the angle
    float dot = q1.w*q2.w + q1.x*q2.x + q1.y*q2.y + q1.z*q2.z;

    // If dot < 0, slerp the opposite rotation (shortest path)
    Quaternion q2b = q2;
    if (dot < 0.0f) { dot = -dot; q2b = {-q2.w, -q2.x, -q2.y, -q2.z}; }

    const float EPSILON = 1e-6f;
    if (dot > 1.0f - EPSILON) {
        // Linear interpolation to avoid divide-by-zero
        return {
            q1.w + t*(q2b.w - q1.w),
            q1.x + t*(q2b.x - q1.x),
            q1.y + t*(q2b.y - q1.y),
            q1.z + t*(q2b.z - q1.z)
        };
    }

    float theta0 = std::acos(dot);
    float theta  = theta0 * t;
    float sinTheta = std::sin(theta);
    float sinTheta0 = std::sin(theta0);

    float s0 = std::cos(theta) - dot * sinTheta / sinTheta0;
    float s1 = sinTheta / sinTheta0;

    return {
        s0*q1.w + s1*q2b.w,
        s0*q1.x + s1*q2b.x,
        s0*q1.y + s1*q2b.y,
        s0*q1.z + s1*q2b.z
    };
}