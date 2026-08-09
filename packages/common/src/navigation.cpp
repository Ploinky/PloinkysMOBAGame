#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <limits>
#include <list>
#include "common/navigation.h"
#include "common/util.h"
#include <fstream>
#include <cstring>
#include <climits>
#include <cfloat>
#include <cmath>
#include "common/pmg_physics.h"
#include "common/PMG_Common.h"
#include <queue>

Vector3 CenterOf(Vector3 v1, Vector3 v2, Vector3 v3) {
	return { (v1.x + v2.x + v3.x) / 3, (v1.y + v2.y + v3.y) / 3, (v1.z + v2.z + v3.z) / 3 };
}

unsigned int polygonId = 0;

bool sorter(const polygon_t* first, const polygon_t* second) {
	float v = first->globalValue - second->globalValue;
	return v == 0 ? 0 : (v < 0 ? 0 : 1);
}

bool operator==(const polygon_t& lhs, const polygon_t& rhs) {
	return lhs.id == rhs.id;
}

bool operator!=(const polygon_t& lhs, const polygon_t& rhs) {
	return lhs.id != rhs.id;
}

NavMesh::NavMesh() {
	from = { 0, 0 };
	to = { 0, 0 };
}

polygon_t* NavMesh::CreatePolygon(Vector3 v1, Vector3 v2, Vector3 v3) {
	polygon_t* p = new polygon_t;
	p->id = polygonId++;
	p->center = CenterOf(v1, v2, v3);
	p->vertices[0] = v1;
	p->vertices[1] = v2;
	p->vertices[2] = v3;

	return p;
}

float NavMesh::Sign(Vector2 p1, Vector3 p2, Vector3 p3) {
	return (p1.x - p3.x) * (p2.z - p3.z) - (p2.x - p3.x) * (p1.y - p3.z);
}

bool NavMesh::PointInTriangle(Vector2 pt, Vector3 v1, Vector3 v2, Vector3 v3) {
	float d1, d2, d3;
	bool has_neg, has_pos;

	d1 = Sign(pt, v1, v2);
	d2 = Sign(pt, v2, v3);
	d3 = Sign(pt, v3, v1);

	has_neg = (d1 < 0) || (d2 < 0) || (d3 < 0);
	has_pos = (d1 > 0) || (d2 > 0) || (d3 > 0);

	return !(has_neg && has_pos);
}

bool NavMesh::PointInMesh(Vector2 pt) {
	for (polygon_t* poly : mesh) {
		if (PointInTriangle(pt, poly->vertices[0], poly->vertices[1], poly->vertices[2])) {
			return true;
		}
	}

	return false;
}

polygon_t* NavMesh::FindPolygonAt(Vector2 pt) {
	for (polygon_t* poly : mesh) {
		if (PointInTriangle(pt, poly->vertices[0], poly->vertices[1], poly->vertices[2])) {
			return poly;
		}
	}

	return nullptr;
}

polygon_t* NavMesh::GetById(unsigned int id) {
	for (polygon_t* poly : mesh) {
		if (poly->id == id) {
			return poly;
		}
	}

	return nullptr;
}

bool NavMesh::IsNeighbour(polygon_t poly, polygon_t potentialNeighbour) {
	return std::find(poly.neighbours.begin(), poly.neighbours.end(), potentialNeighbour.id) != poly.neighbours.end();
}

float NavMesh::Distance(Vector2 a, Vector2 b) {
	return abs(a.x - b.x) + abs(a.y - b.y);
}

float NavMesh::Distance(polygon_t* a, polygon_t* b) {
	return Distance({a->center.x, a->center.z}, {b->center.x, b->center.z});
}

float NavMesh::Cross(const Vector2 v1, const Vector2 v2) {
	return v1.x * v2.y - v1.y * v2.x;
}

float NavMesh::AngleBetween(Vector2 a, Vector2 b) {
	return std::atan2(b.y - a.y, b.x - a.x);
}

void NavMesh::PullString() {

	movePath.clear();

	if (FindPolygonAt(from) == FindPolygonAt(to)) {
		movePath.push_back(to);
		return;
	}

	std::list<Vector3> leftVertices;
	std::list<Vector3> rightVertices;

	Vector3 apex = path.front()->center;

	std::list<portal_t> portals;

	for (auto poly = path.begin(); poly != path.end(); poly++)
	{
		if (poly == std::prev(path.end())) {
			continue;
		}
		portal_t* p = new portal_t;
		p->from = *poly;
		p->to = *std::next(poly);

		Vector3 vf1 = p->from->vertices[0];
		Vector3 vf2 = p->from->vertices[1];
		Vector3 vf3 = p->from->vertices[2];

		Vector3 vt1 = p->to->vertices[0];
		Vector3 vt2 = p->to->vertices[1];
		Vector3 vt3 = p->to->vertices[2];

		Vector3 av1 = { 0, 0, 0 };
		Vector3 av2 = { 0, 0, 0 };

		bool av1b = false;
		if (vf1 == vt1 || vf1 == vt2 || vf1 == vt3)
		{
			av1 = vf1;
			av1b = true;
		}

		if (vf2 == vt1 || vf2 == vt2 || vf2 == vt3)
		{
			if (!av1b) {
				av1 = vf2;
			}
			else {
				av2 = vf2;
			}
		}

		if (vf3 == vt1 || vf3 == vt2 || vf3 == vt3)
		{
			av2 = vf3;
		}

		float av1d = (av1.x - p->from->center.x) * (p->to->center.z - p->from->center.z)
			- (av1.z - p->from->center.z) * (p->to->center.x - p->from->center.x);

		float av2d = (av2.x - p->from->center.x) * (p->to->center.z - p->from->center.z)
			- (av2.z - p->from->center.z) * (p->to->center.x - p->from->center.x);

		if (av1d >= av2d)
		{
			p->left = av1;
			p->right = av2;
		}
		else
		{
			p->left = av2;
			p->right = av1;
		}

		portals.push_back(*p);
	}

	Vector2 currVert = from;
	
	auto leftFrom = portals.begin();
	auto rightFrom = portals.begin();

	auto from = portals.begin();
	// std::cout << "========================================================" << std::endl;
	while (!(currVert == to)) {
		// std::cout << "Apex: " << currVert.x << ", " << currVert.y << std::endl;

		Vector2 tunnelLeft = {from->left.x, from->left.z};
		Vector2 tunnelRight = {from->right.x, from->right.z};

		for (auto portal = from; portal != portals.end(); portal++) {
			// std::cout << "Portal: " << portal->from->id << " -> " << portal->to->id << std::endl;
			Vector2 rightTo = {portal->right.x, portal->right.z};
			Vector2 leftTo = {portal->left.x, portal->left.z};

			float rightNext = Cross(tunnelRight - currVert, rightTo - currVert);
			float rightCross = Cross(rightTo - currVert, tunnelLeft - currVert);

			if (rightCross > 0) {
				// Left is new Apex!
				currVert = tunnelLeft;
				tunnelLeft = {from->left.x, from->left.z};
				tunnelRight = {from->right.x, from->right.z};
				from = leftFrom;
				movePath.push_back(currVert);
				// std::cout << "\tNew apex: " << currVert.x << ", " << currVert.y << std::endl;
				break;
			}
			else if (rightNext <= 0) {
				// Right is new funnel right!
				tunnelRight = rightTo;
				rightFrom = portal;
				// std::cout << "\tRight: " << tunnelRight.x << ", " << tunnelRight.y << std::endl;
			}
			// if tunnel does not tighten, we have a new apex!
			// if we're gonna cross over, do not advance!
			float leftNext = Cross(tunnelLeft - currVert, leftTo - currVert);
			float leftCross = Cross(leftTo - currVert, tunnelRight - currVert);

			if (leftCross < 0) {
				// Right is new Apex!
				currVert = tunnelRight;
				tunnelLeft = {from->left.x, from->left.z};
				tunnelRight = {from->right.x, from->right.z};
				from = rightFrom;
				movePath.push_back(currVert);
				// std::cout << "\tNew apex: " << currVert.x << ", " << currVert.y << std::endl;
				break;
			}
			else if (leftNext >= 0) {
				// Left is new funnel left!
				tunnelLeft = leftTo;
				leftFrom = portal;
				// std::cout << "\tLeft: " << tunnelLeft.x << ", " << tunnelLeft.y << std::endl;
			}

			if (std::next(portal) == portals.end()) {
				rightTo = to;
				leftTo = to;

				float rightNext = Cross(tunnelRight - currVert, rightTo - currVert);
				float rightCross = Cross(rightTo - currVert, tunnelLeft - currVert);

				if (rightCross > 0) {
					// Left is new Apex!
					currVert = tunnelLeft;
					from = leftFrom;
					movePath.push_back(currVert);
					// std::cout << "\tNew apex: " << currVert.x << ", " << currVert.y << std::endl;
					break;
				}
				else if (rightCross == 0) {
					currVert = to;
					movePath.push_back(currVert);
					// std::cout << "\tNew apex: " << currVert.x << ", " << currVert.y << std::endl;
					break;
				}
				else if (rightNext <= 0) {
					// Right is new funnel right!
					tunnelRight = rightTo;
					rightFrom = portal;
					// std::cout << "\tRight: " << tunnelRight.x << ", " << tunnelRight.y << std::endl;
				}

				// if tunnel does not tighten, we have a new apex!
				// if we're gonna cross over, do not advance!
				float leftNext = Cross(tunnelLeft - currVert, leftTo - currVert);
				float leftCross = Cross(leftTo - currVert, tunnelRight - currVert);

				if (leftCross < 0) {
					// Right is new Apex!
					currVert = tunnelRight;
					from = rightFrom;
					movePath.push_back(currVert);
					// std::cout << "\tNew apex: " << currVert.x << ", " << currVert.y << std::endl;
					break;
				}
				else if (leftCross == 0) {
					currVert = to;
					movePath.push_back(currVert);
					// std::cout << "\tNew apex: " << currVert.x << ", " << currVert.y << std::endl;
					break;

				}
				else if (leftNext >= 0) {
					// Left is new funnel left!
					tunnelLeft = leftTo;
					leftFrom = portal;
					// std::cout << "\tLeft: " << tunnelLeft.x << ", " << tunnelLeft.y << std::endl;
				}
			}
		}
	}
}

std::vector<Vector2> NavigationMap::PlanPath(NavigationGridAgent* pAgent, Vector2 from, Vector2 to) {
	m_pMesh->from = from;
	m_pMesh->to = to;

	polygon_t* startPoly = m_pMesh->FindPolygonAt(from);
	polygon_t* endPoly = m_pMesh->FindPolygonAt(to);

	if (startPoly == nullptr || endPoly == nullptr) {
		// What to do when click is not in nav mesh?
		return {};
	}

	for (auto pol : m_pMesh->mesh) {
		pol->globalValue = FLT_MAX;
		pol->localValue = FLT_MAX;
		pol->parent = INT_MAX;
	}

	auto tiebreaker = [](polygon_t* left, polygon_t* right) {
		return left->globalValue > right->globalValue;
	};
	std::priority_queue<polygon_t*, std::vector<polygon_t*>, decltype(tiebreaker)> nodesToTest(tiebreaker);
	std::list<polygon_t*> nodesDone;

	startPoly->globalValue = m_pMesh->Distance(startPoly, endPoly);
	startPoly->localValue = 0;

	nodesToTest.emplace(startPoly);

	polygon_t* currCell;

	while (!nodesToTest.empty())
	{
		currCell = nodesToTest.top();
		nodesToTest.pop();

		for (unsigned int neighbourIndex : currCell->neighbours) {
			polygon_t* neighbour = m_pMesh->GetById(neighbourIndex);

			if (std::find(nodesDone.begin(), nodesDone.end(), neighbour) != nodesDone.end()) {
				continue;
			}

			float newLocal = currCell->localValue + (currCell == startPoly ? m_pMesh->Distance(from, {neighbour->center.x, neighbour->center.z}) : m_pMesh->Distance(currCell, neighbour));
			float newGlobal = newLocal + m_pMesh->Distance({neighbour->center.x, neighbour->center.z}, to);
			if (newLocal + newGlobal < neighbour->globalValue)
			{
				neighbour->parent = currCell->id;
				neighbour->localValue = newLocal;
				neighbour->globalValue = neighbour->localValue + m_pMesh->Distance({neighbour->center.x, neighbour->center.z}, to);
				nodesToTest.emplace(neighbour);
			}
		}

		nodesDone.push_back(currCell);

		if (currCell == endPoly) {
			break;
		}
	}

	m_pMesh->path.clear();

	polygon_t* pathCell = endPoly;

	m_pMesh->path.push_back(pathCell);

	while (pathCell != startPoly) {
		pathCell = m_pMesh->GetById(pathCell->parent);
		if (pathCell == nullptr) {
			return {};
		}
		m_pMesh->path.push_back(pathCell);
	}

	m_pMesh->path.reverse();

	m_pMesh->PullString();

	return m_pMesh->movePath;
}

void NavMesh::FindNeighbours() {
	for (auto poly : mesh) {
		for (auto potentialNeighbour : mesh) {
			if (poly == potentialNeighbour) {
				continue;
			}

			int sharedVertices = 0;

			for (int i = 0; i < 3; i++) {
				for (int j = 0; j < 3; j++) {
					sharedVertices += (poly->vertices[i] == potentialNeighbour->vertices[j]);
				}
			}

			if (sharedVertices > 1) {
				poly->neighbours.push_back(potentialNeighbour->id);
			}
		}
	}
}

void NavMesh::LoadFromData(std::list<std::string> data) {
	mesh.clear();
	std::vector<Vector3> vertices;

	for (std::string line : data ) {
		std::list<std::string> tokens = Util::SplitString(line, " ");
		//printf("%s\n", line.c_str());

		std::string lineType = tokens.front();
		tokens.pop_front();

		if (!std::strcmp(lineType.c_str(), "f")) {
			int v1Index = std::stoi(tokens.front());
			tokens.pop_front();
			int v2Index = std::stoi(tokens.front());
			tokens.pop_front();
			int v3Index = std::stoi(tokens.front());
			tokens.pop_front();
			mesh.push_back(CreatePolygon(vertices[v1Index], vertices[v2Index], vertices[v3Index]));
		}
		else if (!std::strcmp(lineType.c_str(), "v")) {
			float x = std::stof(tokens.front());
			tokens.pop_front();
			float y = std::stof(tokens.front());
			tokens.pop_front();
			float z = std::stof(tokens.front());
			tokens.pop_front();

			Vector3 v;
			v.x = x;
			v.y = y;
			v.z = z;
			vertices.push_back(v);
		}
	}

	FindNeighbours();
}

void NavMesh::LoadFromFile(std::string mapName) {
	std::string mapFileName = "./maps/";
	mapFileName.append(mapName).append(".nvm");

	std::ifstream file(mapFileName);

	if (!file.is_open()) {
		throw new std::runtime_error("Could not open file");
	}

	mesh.clear();
	std::vector<Vector3> vertices;

	for (std::string line; std::getline(file, line); ) {
		std::list<std::string> tokens = Util::SplitString(line, " ");
		//printf("%s\n", line.c_str());

		std::string lineType = tokens.front();
		tokens.pop_front();

		if (!std::strcmp(lineType.c_str(), "f")) {
			int v1Index = std::stoi(tokens.front());
			tokens.pop_front();
			int v2Index = std::stoi(tokens.front());
			tokens.pop_front();
			int v3Index = std::stoi(tokens.front());
			tokens.pop_front();
			mesh.push_back(CreatePolygon(vertices[v1Index], vertices[v2Index], vertices[v3Index]));
		}
		else if (!std::strcmp(lineType.c_str(), "v")) {
			float x = std::stof(tokens.front());
			tokens.pop_front();
			float y = std::stof(tokens.front());
			tokens.pop_front();
			float z = std::stof(tokens.front());
			tokens.pop_front();

			Vector3 v;
			v.x = x;
			v.y = y;
			v.z = z;
			vertices.push_back(v);
		}
	}

	FindNeighbours();
}

PathResult_t NavigationMap::RequestPath(NavigationGridAgent* pAgent, Vector2 from, Vector2 to) {
	if (!m_pMesh->PointInMesh(to) || !CanMoveTo(pAgent, m_pGrid->GetCellAt(to.x, to.y), true)) {
		PathResult_t result;
		result.eResultType = EPathResultType::OCCUPIED;
		result.vecVec2Path = {};
		return result;
	}

	std::vector<Vector2> vecVec2LongPath = PlanPath(pAgent, from, to);

	if (vecVec2LongPath.size() < 1) {
		PathResult_t result;
		result.eResultType = EPathResultType::UNREACHABLE;
		result.vecVec2Path = {};
		return result;
	}

	Vector2 vec2Intermediate = vecVec2LongPath.front();

	std::vector<Vector2> vecVec2ShortPath = GetGridPath(pAgent, from, vec2Intermediate);

	for (int i = 1; i < vecVec2LongPath.size(); i++) {
		vecVec2ShortPath.push_back(vecVec2LongPath[i]);
	}

	std::vector<Vector2> vecVec2FinalPath = GetSmoothPath(pAgent, vecVec2ShortPath);

	PathResult_t result;
	result.eResultType = EPathResultType::FOUND;
	result.vecVec2Path = vecVec2FinalPath;
	return result;
}

bool NavigationMap::CanMoveTo(NavigationGridAgent* pAgent, NavigationCell* pCell, bool bIgnoreMoving) {
	if (!pCell->IsWalkable) {
		return false;
	}

	for (NavigationGridAgent* pOtherAgent : m_vecAgents) {
		if (pOtherAgent == pAgent) {
			continue;
		}

		if (bIgnoreMoving && pOtherAgent->bIgnoreForPathing) {
			continue;
		}

		if (CompareFloat(pOtherAgent->nCollisionRadius, 0)) {
			return false;
		}
		if ((pOtherAgent->position - Vector2(pCell->X + m_pGrid->CellWidth / 2, pCell->Y + m_pGrid->CellHeight / 2)).Length() <= pOtherAgent->nCollisionRadius + 1) {
			return false;
		}
	}

	return true;
}

StepResult_t NavigationMap::Step(NavigationGridAgent* pAgent, Vector2 vec2CurrPos, std::vector<Vector2> path, float fDist) {
	if(path.empty()) {
		// TODO is this blocked = false?
		return {false, vec2CurrPos};
	}

	Vector2 vec2Move = path.front() - vec2CurrPos;

	if (vec2Move.Length() > fDist) {
		vec2Move = vec2Move.ScaleToLength(fDist);
	}

	Vector2 vec2NewPos = vec2CurrPos + vec2Move;

	NavigationCell* pCell = m_pGrid->GetCellAt(vec2NewPos.x, vec2NewPos.y);

	if (!CanMoveTo(pAgent, pCell, false)) {
		return {true, vec2CurrPos};
	}

	if (CompareFloat((vec2NewPos - path.front()).Length(), 0)) {
		std::vector<Vector2> path;
		for(int i = 1; i < path.size(); i++) {
			path.push_back(path.at(i));
		}
		path = path;
	}

	return {false, vec2NewPos};
}

NavigationGridAgent* NavigationMap::CreateAgent() {
	NavigationGridAgent* pAgent = new NavigationGridAgent();
	m_vecAgents.push_back(pAgent);
	return pAgent;
}





// =============== GRID NAVIGATION ===============
NavigationCellGrid::NavigationCellGrid(NavMesh* navMesh) {
	float maxX = -100000;
	float minX = 100000;
	float maxY = -100000;
	float minY = 100000;

	for(auto pol : navMesh->mesh) {
		for(auto vert : pol->vertices) {
			maxX = std::max(vert.x, maxX);
			minX = std::min(vert.x, minX);
			maxY = std::max(vert.z, maxY);
			minY = std::min(vert.z, minY);
		}
	}

	GridWidth = maxX - minX;
	GridHeight = minY - maxY;

	CellWidth = 50;
	CellHeight = -50;

	CellCountX = GridWidth / CellWidth;
	CellCountY = std::abs(GridHeight / CellHeight);

	GridCenterX = 0;
	GridCenterY = 0;

	Cells = new NavigationCell*[CellCountX * CellCountY];

	for (int y = 0; y < CellCountY; y++) {
		for (int x = 0; x < CellCountX; x++) {
			NavigationCell* cell = new NavigationCell();
			cell->X = x * CellWidth + minX;
			cell->Y = y * CellHeight + maxY;
			cell->IsOpen = true;
			cell->IsWalkable = true;

			if (
				!navMesh->PointInMesh({ (float)cell->X, (float)cell->Y})
				&& !navMesh->PointInMesh({ (float)cell->X + CellWidth, (float)cell->Y})
				&& !navMesh->PointInMesh({ (float)cell->X + CellWidth, (float)cell->Y + CellHeight})
				&& !navMesh->PointInMesh({ (float)cell->X, (float)cell->Y + CellHeight})
				
			) {
				cell->IsWalkable = false;
			}

			cell->CalculateNeighbours(CellCountX, CellCountY, CellWidth, CellHeight, GridCenterX, GridCenterY);

			SetCellAt(x * CellWidth, y * CellHeight, cell);
		}
	}
}

const float D = 1, D2 = sqrt(2);
float NavigationCellGrid::Heuristic(NavigationCell* pStartCell, NavigationCell* pNeighbourCell, NavigationCell* pEndCell) {
	float x = abs(pNeighbourCell->X - pEndCell->X);
	float y = abs(pNeighbourCell->Y - pEndCell->Y);
	float cross = Cross(pStartCell, pNeighbourCell, pEndCell);
	return (x + y) + cross;
}

float NavigationCellGrid::Distance(NavigationCell* a, NavigationCell* b) {
	float x = b->X - a->X;
	float y = b->Y - a->Y;
	return sqrt(x * x + y * y);
}	

void NavigationCell::CalculateNeighbours(int gridWidth, int gridHeight, int cellWidth, int cellHeight, int gridOffsetX, int gridOffsetY) {
	Neighbours.clear();
	bool allowDiagonal = false;

	int x = (X + gridOffsetX) / cellWidth;
	int y = (Y + gridOffsetY) / cellHeight;

	if (x > 0) {
		if (y > 0 && allowDiagonal) {
			Neighbours.push_back(x - 1 + (y - 1) * gridWidth);
		}

		Neighbours.push_back(x - 1 + y * gridWidth);

		if (y < gridHeight - 1 && allowDiagonal) {
			Neighbours.push_back(x - 1 + (y + 1) * gridWidth);
		}
	}
	if (y > 0) {
		Neighbours.push_back(x + (y - 1) * gridWidth);
	}

	if (y < gridHeight - 1) {
		Neighbours.push_back(x + (y + 1) * gridWidth);
	}

	if (x < gridWidth - 1) {
		if (y > 0 && allowDiagonal) {
			Neighbours.push_back(x + 1 + (y - 1) * gridWidth);
		}

		Neighbours.push_back(x + 1 + y * gridWidth);

		if (y < gridHeight - 1 && allowDiagonal) {
			Neighbours.push_back(x + 1 + (y + 1) * gridWidth);
		}
	}

	for(auto n : Neighbours) {
		if(n > gridWidth * gridHeight) {
			Neighbours.clear();
		}
	}
}


float NavigationCellGrid::Cross(const NavigationCell* v1, const NavigationCell* v2, const NavigationCell* v3) {
	float dx1 = v1->X - v3->X;
	float dy1 = v1->Y - v3->Y;
	float dx2 = v2->X - v3->X;
	float dy2 = v2->Y - v3->Y;
	return abs(dx1 * dy2 - dx2 * dy1);
}

bool NavigationMap::IsClearPath(NavigationGridAgent* pAgent, Vector2 vec2Start, const NavigationCell* node2) {
	float x1 = vec2Start.x;
	float y1 = vec2Start.y;
	float x2 = node2->X + m_pGrid->CellWidth / 2;
	float y2 = node2->Y + m_pGrid->CellHeight / 2;

	float dx = (x2 - x1) / m_pGrid->CellWidth;
	float dy = (y2 - y1) / m_pGrid->CellHeight;

	float sx = dx == 0 ? 9999.0f : std::sqrt(1 + std::pow(dy/dx, 2));
	float sy = dy == 0 ? 9999.0f : std::sqrt(1 + std::pow(dx/dy, 2));

	float x = x1 / m_pGrid->CellWidth;
	float y = y1 / m_pGrid->CellHeight;
	
	NavigationCell* square = m_pGrid->GetCellAt(x1, y1);
	
	float fx = (x - ((int) x));
	float fy = (y - ((int) y));
	float distX = sx * (dx > 0 ? 1 - fx : fx);
	float distY = sy * (dy > 0 ? 1 - fy : fy);

	while (square != nullptr && square != node2) {
		if (x < 0 || x >= m_pGrid->GridWidth || y < 0 || y <= m_pGrid->GridHeight) {
			return false; // Path goes out of bounds
		}

		if (!CanMoveTo(pAgent, square, true)) {
			return false; // Path is blocked
		}

		if(std::abs(distX) < std::abs(distY)) {
			if(x == x1) {
				x += dx > 0 ? 1 : -1;
				distX += distX;
			} else {
				x += dx > 0 ? 1 : -1;
				distX += sx;
			}
		} else {
			if(y == y1) {
				y += dy > 0 ? 1 : -1;
				distY += distY;
			} else {
				y += dy > 0 ? 1 : -1;
				distY += sy;
			}
		}

		square = m_pGrid->GetCellAt(x * m_pGrid->CellWidth, y * m_pGrid->CellHeight);
	}

	return true;
}

void NavigationMap::Reset() {
	m_pGrid->Reset();
}

void NavigationCellGrid::Reset() {
	for (int i = 0; i < CellCountX * CellCountY; i++) {
		NavigationCell* cell = Cells[i];
		cell->GlobalValue = FLT_MAX;
		cell->LocalValue = FLT_MAX;
		cell->Parent = INT_MAX;
		cell->Done = false;
	}
}


auto tiebreaker = [](NavigationCell* left, NavigationCell* right) {
	return left->GlobalValue > right->GlobalValue;
};

std::priority_queue<NavigationCell*, std::vector<NavigationCell*>, decltype(tiebreaker)> nodesToTest(tiebreaker);

std::vector<Vector2> NavigationMap::GetLongPath(NavigationGridAgent* pAgent, Vector2 from, Vector2 to) {
	return PlanPath(pAgent, from, to);
}

#ifdef _DEBUG
DebugPath_t NavigationMap::GetDebugPath(NavigationGridAgent* pAgent, Vector2 from, Vector2 to) {
	DebugPath_t result;

	result.vecVec2LongPath = PlanPath(pAgent, from, to);

	if (result.vecVec2LongPath.size() < 1) {
		return {};
	}

	Vector2 vec2Intermediate = result.vecVec2LongPath.front();

	std::vector<Vector2> vecVec2ShortPath = GetGridPath(pAgent, from, vec2Intermediate);
	result.vecVec2ShortPath = vecVec2ShortPath;

	std::vector<Vector2> vecVec2SmoothPath = GetSmoothPath(pAgent, vecVec2ShortPath);

	for (int i = 1; i < result.vecVec2LongPath.size(); i++) {
		vecVec2SmoothPath.push_back(result.vecVec2LongPath[i]);
	}
	result.vecVec2FinalPath = vecVec2SmoothPath;
	return result;
}
#endif

std::vector<Vector2> NavigationMap::GetGridPath(NavigationGridAgent* pAgent, Vector2 from, Vector2 to) {
	return GetGridPath(pAgent, from, to, false);
}

std::vector<Vector2> NavigationMap::GetSmoothPath(NavigationGridAgent* pAgent, std::vector<Vector2> vecVec2CoarsePath) {
	std::vector<Vector2> smoothedPath;
	if (vecVec2CoarsePath.empty()) {
		return {};
	}
	Vector2 smoothAnchor = pAgent->position;
	int i = 0;
	while (i < vecVec2CoarsePath.size()) {
		NavigationCell* pCell = m_pGrid->GetCellAt(vecVec2CoarsePath[i].x, vecVec2CoarsePath[i].y);
		if (IsClearPath(pAgent, smoothAnchor, pCell)) {
			i++;
		}
		else {
			Vector2 vec2NewPt = { vecVec2CoarsePath[i - 1].x, vecVec2CoarsePath[i - 1].y};
			if (!smoothedPath.empty() && vec2NewPt == smoothedPath.back()) {
				i++;
				continue;
			}
			smoothedPath.push_back(vec2NewPt);
			smoothAnchor = smoothedPath.back();
		}
	}

	smoothedPath.push_back(vecVec2CoarsePath.back());
	return smoothedPath;
}

std::vector<Vector2> NavigationMap::GetGridPath(NavigationGridAgent* pAgent, Vector2 from, Vector2 to, bool bIgnoreOpen) {
	m_pGrid->Reset();
	NavigationCell* startCell = m_pGrid->GetCellAt(from.x, from.y);
	NavigationCell* anchor = startCell;
	NavigationCell* endCell = m_pGrid->GetCellAt(to.x, to.y);

	if (startCell == nullptr || endCell == nullptr) {
		return {};
	}

	if(!startCell->IsWalkable || !endCell->IsWalkable) {
		return {};
	}

	if(startCell == endCell) {
		return {};
	}

	m_pGrid->currCell = startCell;

	startCell->GlobalValue = 0;
	startCell->LocalValue = 0;

	while (!nodesToTest.empty()) {
		nodesToTest.pop();
	}
	nodesToTest.emplace(startCell);

	int c = 0;
	while (!nodesToTest.empty()) {
		m_pGrid->currCell = nodesToTest.top();
		nodesToTest.pop();

		for (unsigned int neighbourIndex : m_pGrid->currCell->Neighbours) {
			c++;
			NavigationCell* neighbour = m_pGrid->Cells[neighbourIndex];

			if (m_pGrid->currCell == neighbour || !neighbour->IsWalkable || !CanMoveTo(pAgent, neighbour, true)) {
				continue;
			}

			float newLocal = m_pGrid->currCell->LocalValue + D;

			if (newLocal < neighbour->LocalValue)
			{
				neighbour->Parent = m_pGrid->currCell->Index;
				neighbour->LocalValue = newLocal;
				neighbour->GlobalValue = newLocal + m_pGrid->Heuristic(startCell, neighbour, endCell);
				nodesToTest.emplace(neighbour);
			}
		}

		if (m_pGrid->currCell == endCell) {
			break;
		}
	}

	std::vector<NavigationCell*> path;

	NavigationCell* pathCell = endCell;

	path.push_back(pathCell);

	while (pathCell != startCell) {
		if (pathCell->Parent == INT_MAX || pathCell == m_pGrid->Cells[pathCell->Parent] ) {
			return {};
		}
		pathCell = m_pGrid->Cells[pathCell->Parent];
		if (pathCell == nullptr) {
			return {};
		}
		path.push_back(pathCell);
	}

	std::reverse(path.begin(), path.end());

	std::vector<Vector2> vecVec2CoarsePath;

	for (NavigationCell* pCell : path) {
		vecVec2CoarsePath.push_back({ pCell->X + m_pGrid->CellWidth / 2, pCell->Y + m_pGrid->CellHeight / 2 });
	}

	vecVec2CoarsePath.push_back(to);

	return vecVec2CoarsePath;
}

NavigationCell* NavigationCellGrid::GetCellAt(float x, float y) {
	int index = ((int)(x + GridCenterX) / CellWidth) + ((int)(y + GridCenterY) / CellHeight * CellCountX);

	if (index < 0 || index > CellCountX * CellCountY) {
		return nullptr;
	}

	return Cells[index];
}

void NavigationCellGrid::SetCellAt(float x, float y, NavigationCell* cell) {
	int index = (x + GridCenterX) / CellWidth + (y + GridCenterY) / CellHeight * CellCountX;

	if (index < 0 || index > CellCountX * CellCountY) {
		return;
	}

	Cells[index] = cell;
	cell->Index = index;
}

Vector2 NavigationMap::ClosestFreePosition(NavigationGridAgent* pAgent, Vector2 vec2Target) {
	// first, clamp coordinates to map bounds - everything else is nonesense
	vec2Target.x = std::fmax(0.0f, std::fmin(vec2Target.x, m_pGrid->GridWidth));
	vec2Target.y = std::fmin(0.0f, std::fmax(vec2Target.y, m_pGrid->GridHeight));

	// this is the cell we would like to get if it's free
	NavigationCell* pPreferredCell = m_pGrid->GetCellAt(vec2Target.x, vec2Target.y);

	if (pPreferredCell == nullptr) {
		// TODO this must be no inside the grid?
		return pAgent->position;
	}

	// just extend outward in circles.... I can imagine this has lots of problems
	// but it just has to be good enough for now, and probably will be in 99.99% of cases
	// might look in the direction of the player first?
	int nWest = pPreferredCell->Index - 1;
	int nNorthWest = nWest - m_pGrid->CellCountX;
	int nNorth = nNorthWest + 1;
	int nNorthEast = nNorth + 1;
	int nEast = nNorthEast + m_pGrid->CellCountX;
	int nSouthEast = nEast + m_pGrid->CellCountX;
	int nSouth = nSouthEast - 1;
	int nSouthWest = nSouth - 1;

	auto fnCheckCell = [this, pAgent](int nIndex) {
		return nIndex > 0 && nIndex < m_pGrid->CellCountX + m_pGrid->CellCountX * m_pGrid->CellCountY && m_pGrid->Cells[nIndex] && m_pGrid->Cells[nIndex]->IsWalkable && CanMoveTo(pAgent, m_pGrid->Cells[nIndex], true);
	};

	auto fnToCoord = [this](int nIndex) {
		return Vector2(m_pGrid->Cells[nIndex]->X + m_pGrid->CellWidth / 2.0f, m_pGrid->Cells[nIndex]->Y + m_pGrid->CellHeight / 2.0f);
	};

	while (nNorthWest <= m_pGrid->CellCountX + m_pGrid->CellCountX * m_pGrid->CellCountY && nSouthEast >= 0) {
		if (fnCheckCell(nWest)) {
			return fnToCoord(nWest);
		}

		if (fnCheckCell(nNorthWest)) {
			return fnToCoord(nNorthWest);
		}

		if (fnCheckCell(nNorth)) {
			return fnToCoord(nNorth);
		}

		if (fnCheckCell(nNorthEast)) {
			return fnToCoord(nNorthEast);
		}

		if (fnCheckCell(nEast)) {
			return fnToCoord(nEast);
		}

		if (fnCheckCell(nSouthEast)) {
			return fnToCoord(nSouthEast);
		}

		if (fnCheckCell(nSouth)) {
			return fnToCoord(nSouth);
		}

		if (fnCheckCell(nSouthWest)) {
			return fnToCoord(nSouthWest);
		}

		// TODO this is for sure wrong btw but it's better than nothing.
		nWest -= 1;
		nNorthWest = nWest - m_pGrid->CellCountX;
		nNorth = nNorthWest + 1;
		nNorthEast = nNorth + 1;
		nEast = nNorthEast + m_pGrid->CellCountX;
		nSouthEast = nEast + m_pGrid->CellCountX;
		nSouth = nSouthEast - 1;
		nSouthWest = nSouth - 1;
	}

	return vec2Target;
}