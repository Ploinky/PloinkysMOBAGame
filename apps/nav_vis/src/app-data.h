#pragma once

#include <common/navigation.h>

typedef struct AppData {
	NavigationMap* pMap;
	NavigationGridAgent* pAgent;
	Vector2 vec2Target;
	std::vector<Vector2> path;
 	std::vector<Vector2> vecVec2LongPath;
	std::vector<Vector2> vecVec2ShortPath;
	Vector2 vec2CurrPos = { 1000, -1000 };
	std::vector<Vector2> vecVec2CoarsePath = {};
} AppData_t;