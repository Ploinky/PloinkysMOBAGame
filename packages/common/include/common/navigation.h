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
	std::list<Vector3> movePath;
	Vector3 from;
	Vector3 to;

	NavMesh();
	void LoadFromFile(std::string mapName);
	void LoadFromData(std::list<std::string> data);
	bool PointInMesh(Vector3 pt);

	void FindNeighbours();
	void PullString();
	polygon_t* FindPolygonAt(Vector3 pt);
	polygon_t* GetById(unsigned int id);
	polygon_t* CreatePolygon(Vector3 v1, Vector3 v2, Vector3 v3);
	float Sign(Vector3 p1, Vector3 p2, Vector3 p3);
	bool PointInTriangle(Vector3 pt, Vector3 v1, Vector3 v2, Vector3 v3);
	bool IsNeighbour(polygon_t poly, polygon_t potentialNeighbour);
	float Distance(Vector3 a, Vector3 b);
	float Distance(polygon_t* a, polygon_t* b);
	float Cross(const Vector3 v1, const Vector3 v2);
	float AngleBetween(Vector3 a, Vector3 b);
};

typedef struct {
	polygon_t* from;
	polygon_t* to;
	Vector3 left;
	Vector3 right;
} portal_t;






// =============== GRID NAVIGATION ===============
class NavigationCell {
public:
	float X;
	float Y;
	bool IsWalkable;
	bool IsOpen;
	uint64_t UnitId;
	int Index;
	std::vector<int> Neighbours;

	// ==== Navigation ====
	float GlobalValue;
	float LocalValue;
	int Parent;
	bool Done;

	void CalculateNeighbours(int gridWidth, int gridHeight, int cellWidth, int cellHeight, int gridOffsetX, int gridOffsetY);
};

class NavigationCellGrid {
public:
	NavigationCellGrid(NavMesh* navMesh);


	NavigationCell* GetCellAt(float x, float y);
	void SetCellAt(float x, float y, NavigationCell* cell);
	void Reset();

	int GridWidth;
	int GridHeight;
	int GridCenterX;
	int GridCenterY;
	int CellWidth;
	int CellHeight;
	int CellCountX;
	int CellCountY;
	NavigationCell** Cells;

	float Distance(NavigationCell* a, NavigationCell* b);
	float Heuristic(NavigationCell* a, NavigationCell* b, NavigationCell* c);
	float Cross(const NavigationCell* v1, const NavigationCell* v2, const NavigationCell* v3);
	NavigationCell* currCell;
};

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

class NavigationMap {
public:
	std::vector<Vector2> GetPath(NavigationGridAgent* pAgent, Vector2 from, Vector2 to);
	StepResult_t Step(NavigationGridAgent* pAgent, Vector2 vec2CurrPos, float fDist);
	NavigationGridAgent* CreateAgent();
	std::vector<Vector2> GetGridPath(NavigationGridAgent* pAgent, Vector2 from, Vector2 to);
	std::vector<Vector2> GetGridPath(NavigationGridAgent* pAgent, Vector2 from, Vector2 to, bool bIgnoreOpen);
	bool IsClearPath(NavigationGridAgent* pAgent, Vector2 vec2Start, const NavigationCell* node2);
	NavMesh* m_pMesh;
	NavigationCellGrid* m_pGrid;
	void Reset();
	std::vector<NavigationGridAgent*> m_vecAgents;
	bool CanMoveTo(NavigationGridAgent* pAgent, NavigationCell* pCell, bool bIgnoreMoving);
	std::vector<Vector2> GetCoarseGridPath(NavigationGridAgent* pAgent, Vector2 from, Vector2 to);

private:
	std::list<Vector3> PlanPath(NavigationGridAgent* pAgent, Vector3 from, Vector3 to);
};


#endif