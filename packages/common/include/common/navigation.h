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

typedef struct {
	std::list<Vector3> path;
	Vector3 target;
	Vector3 position;
} nav_agent_t;

class NavigationMap;

typedef struct {
	uint64_t UnitId;
	std::vector<Vector2> path;
	Vector3 target;
	Vector3 position;
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

	nav_agent_t* AddAgent(Vector3 startPosition);
	Vector2 GetNextStep(nav_agent_t* agent);

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

	std::vector<nav_agent_t*> agents_;
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


class NavigationMap {
public:
	std::vector<Vector2> GetPath(NavigationGridAgent* pAgent, Vector2 from, Vector2 to);
	std::vector<Vector2> GetGridPath(NavigationGridAgent* pAgent, Vector2 from, Vector2 to);
	std::vector<Vector2> GetGridPath(NavigationGridAgent* pAgent, Vector2 from, Vector2 to, bool bIgnoreOpen);
	bool IsClearPath(NavigationGridAgent* pAgent, NavigationCell* node1, const NavigationCell* node2);
	Vector2 Step(NavigationGridAgent* pAgent, Vector2 vec2CurrPos, float fDist);
	NavigationGridAgent* CreateAgent();
	bool CheckCell(NavigationGridAgent* pAgent, NavigationCell* pCell);

	NavigationCellGrid* m_pGrid;
	NavMesh* m_pMesh;

	std::vector<NavigationGridAgent*> m_vecAgents;
private:
	std::list<Vector3> PlanPath(NavigationGridAgent* pAgent, Vector3 from, Vector3 to);

};

#endif