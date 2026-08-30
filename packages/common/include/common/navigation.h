#ifndef _NAVIGATION_H_
#define _NAVIGATION_H_

#include <vector>
#include <string>
#include <list>
#include <stdint.h>
#include <map>

#include <common/pmg_types.h>
#include "pmg_physics.h"
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






// =============== GRID NAVIGATION ===============

typedef struct {
	float x;
	float y;
	float radius;
} GridBlocker_t;
typedef EngineHandle HGridBlocker;

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

enum class EEdgeType { LINE, ARC };

struct Edge_t {
    EEdgeType eType;
    Vector2 start;
    Vector2 end;

    // only used if type == Arc
    Vector2 arcCenter;
    float arcRadius;
    bool arcClockwise; // sweep direction
};
#ifdef _DEBUG
typedef struct {
	std::vector<Vector2> vecVec2LongPath;
	std::vector<Vector2> vecVec2ShortPath;
	std::vector<Vector2> vecVec2FinalPath;
	std::vector<Vector2> vecNodes;
	std::vector<Edge_t> vecEdges;
} DebugPath_t;
#endif

enum class EPathResultType {
	FOUND,
	UNREACHABLE,
	OCCUPIED
};

typedef struct {
	EPathResultType eResultType;
	std::vector<Vector2> vecVec2Path;
} PathResult_t;


typedef struct {
	HGridBlocker hBlocker;
	Vector2 position;
} Collision_t;

typedef struct {
	HGridBlocker hBlocker;
	Vector2 vec2Pos;
	float radius;
} Node_t;

class NavigationMap {
public:
	PathResult_t RequestPath(Vector2 from, Vector2 to);
	StepResult_t Step(Vector2 vec2CurrPos, std::vector<Vector2> vecVec2Path, float fDist);
	NavMesh* m_pMesh;
	void Reset();
	std::vector<Vector2> GetLongPath(Vector2 from, Vector2 to);
	Vector2 ClosestFreePosition(Vector2 vec2Target);

#ifdef _DEBUG
	DebugPath_t GetDebugPath(Vector2 from, Vector2 to);
#endif

	std::map<HGridBlocker, GridBlocker_t> m_blockers;
	HGridBlocker AddBlocker(float x, float y, float radius);
	void RemoveBlocker(HGridBlocker hBlocker);
private:
	std::vector<Vector2> PlanPath(Vector2 from, Vector2 to);

	
	Collision_t CheckEdge(Vector2 vec2Position, Vector2 vec2Destination, std::vector<Vector2>& nodes, std::vector<Node_t>& vecGraphNodes, std::vector<Edge_t>& edges, float collDist);
	void AddBlockerToGraph(std::vector<Vector2>& nodes, std::vector<Edge_t>& edges, std::vector<Node_t> vecGraphNodes, Node_t nodeToAdd, float collDist);
};


#endif