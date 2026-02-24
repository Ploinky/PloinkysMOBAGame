#pragma once

#include "vector2.h"

bool CompareFloat(float a, float b);
float CalculateAngle(Vector2 from, Vector2 to);
Vector2 CalculateDirectionVector(Vector2 from, float angle);
float ToRadians(float degrees);
float ToDegrees(float radians);