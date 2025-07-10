#include "calculation.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include <limits>
#include <cmath>

bool CompareFloat(float a, float b) {
	return abs(a - b) <= std::numeric_limits<float>::epsilon();
}

float CalculateAngle(Vector2 from, Vector2 to) {
	float dx = to.x - from.x;
	float dy = -(to.y - from.y);

	return ToDegrees(atan2(dy, dx));
}

float ToRadians(float degrees) {
	return degrees * (M_PI / 180);
}

float ToDegrees(float radians) {
	return radians * 180.0f / M_PI;
}