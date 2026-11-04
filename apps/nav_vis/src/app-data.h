#pragma once

#include <common/navigation.h>

typedef struct AppData {
	NavigationMap* pMap;
	Vector2 vec2Target;
	std::vector<PathSegment_t> path;
 	std::vector<Vector2> vecVec2LongPath;
	std::vector<PathSegment_t> vecVec2ShortPath;
	Vector2 vec2CurrPos = { 1000, -1000 };
	std::vector<Vector2> vecVec2CoarsePath = {};
#ifdef _DEBUG
	DebugPath_t debugPath;
#endif
} AppData_t;