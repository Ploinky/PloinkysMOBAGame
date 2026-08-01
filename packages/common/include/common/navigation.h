#ifndef _NAVIGATION_H_
#define _NAVIGATION_H_

#include <vector>
#include <string>
#include <list>
#include "pmg_physics.h"
#include <stdint.h>
#include "common/util/frame-timer.h"

Vector3 CenterOf(Vector3 v1, Vector3 v2, Vector3 v3);

extern unsigned int polygonId;

typedef struct {
	unsigned int id;
	Vector3 center;
	Vector3 vertices[3];
	std::vector<unsigned int> neighbours;
	float globalValue;
	float localValue;
	unsigned int parent;
} polygon_t;

bool sorter(const polygon_t* first, const polygon_t* second);

bool operator==(const polygon_t& lhs, const polygon_t& rhs);

bool operator!=(const polygon_t& lhs, const polygon_t& rhs);

class NavigationMap;

typedef struct {
	uint64_t UnitId;
	std::vector<Vector2> path;
	Vector2 target;
	Vector2 position;
	int nCollisionRadius;
	bool IgnoreCollision;
} NavigationGridAgent;

class NavMesh {
public:
	std::vector<polygon_t*> mesh;
	std::list<polygon_t*> path;
	std::vector<Vector2> movePath;
	Vector2 from;
	Vector2 to;

	NavMesh();
	void LoadFromFile(std::string mapName);
	void LoadFromData(std::list<std::string> data);
	bool PointInMesh(Vector2 pt);

	void FindNeighbours();
	void PullString();
	polygon_t* FindPolygonAt(Vector2 pt);
	polygon_t* GetById(unsigned int id);
	polygon_t* CreatePolygon(Vector3 v1, Vector3 v2, Vector3 v3);
	float Sign(Vector2 p1, Vector3 p2, Vector3 p3);
	bool PointInTriangle(Vector2 pt, Vector3 v1, Vector3 v2, Vector3 v3);
	bool IsNeighbour(polygon_t poly, polygon_t potentialNeighbour);
	float Distance(Vector2 a, Vector2 b);
	float Distance(polygon_t* a, polygon_t* b);
	float Cross(const Vector2 v1, const Vector2 v2);
	float AngleBetween(Vector2 a, Vector2 b);
};

typedef struct {
	polygon_t* from;
	polygon_t* to;
	Vector3 left;
	Vector3 right;
} portal_t;


/**
 * Result of a unit taking a step (see NavigationMap::Step).
 * If bBlocked, the unit could not take a step along it's path because
 * it was blocked. The unit should probably attempt to re-reoute.
 * If bBlocked is false, the unit was able to complete the step.
 * In either case, vec2Pos will be the new position of the unit.
 */
typedef struct {
	bool bBlocked;
	Vector2 vec2Pos;
} StepResult_t;

#ifdef _DEBUG
typedef struct {
	std::vector<Vector2> vecVec2LongPath;
	std::vector<Vector2> vecVec2ShortPath;
	std::vector<Vector2> vecVec2FinalPath;
} Paths_t;
#endif

class NavigationMap {
public:
	std::vector<Vector2> GetPath(NavigationGridAgent* pAgent, Vector2 from, Vector2 to);
	StepResult_t Step(NavigationGridAgent* pAgent, Vector2 vec2CurrPos, float fDist);
	NavigationGridAgent* CreateAgent();
	std::vector<Vector2> GetGridPath(NavigationGridAgent* pAgent, Vector2 from, Vector2 to);
	std::vector<Vector2> GetGridPath(NavigationGridAgent* pAgent, Vector2 from, Vector2 to, bool bIgnoreOpen);
	NavMesh* m_pMesh;
	void Reset();
	std::vector<NavigationGridAgent*> m_vecAgents;
	std::vector<Vector2> GetCoarseGridPath(NavigationGridAgent* pAgent, Vector2 from, Vector2 to);

	std::vector<Vector2> SmoothPath(NavigationGridAgent* pAgent, const std::vector<Vector2>& vecShortPath, Vector2 start, Vector2 end);
std::vector<Vector2> RefinePath(NavigationGridAgent* pAgent, const std::vector<Vector2>& vecLongPath, Vector2 from, Vector2 to);

#ifdef _DEBUG
	Paths_t GetDebugPath(NavigationGridAgent* pAgent, Vector2 from, Vector2 to);
#endif

private:
	std::vector<Vector2> PlanPath(NavigationGridAgent* pAgent, Vector2 from, Vector2 to);
};


#endif