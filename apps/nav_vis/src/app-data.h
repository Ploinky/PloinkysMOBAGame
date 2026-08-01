#pragma once

#include <common/navigation.h>

typedef struct AppData {
	NavigationMap* pMap;
	NavigationGridAgent* pAgent;
	std::vector<Vector2> vecVec2Blocker = { { 1500, -1500} };

	Vector2 vec2CurrPos = { 1000, -1000 };
	std::vector<Vector2> vecVec2CoarsePath = {};
} AppData_t;