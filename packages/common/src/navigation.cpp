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

std::vector<Vector2> NavigationMap::PlanPath(Vector2 from, Vector2 to) {
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

PathResult_t NavigationMap::RequestPath(Vector2 from, Vector2 to) {
	if (!m_pMesh->PointInMesh(to)) {
		PathResult_t result;
		result.eResultType = EPathResultType::OCCUPIED;
		result.vecVec2Path = {};
		return result;
	}

	std::vector<Vector2> vecVec2LongPath = PlanPath(from, to);

	if (vecVec2LongPath.size() < 1) {
		PathResult_t result;
		result.eResultType = EPathResultType::UNREACHABLE;
		result.vecVec2Path = {};
		return result;
	}

	Vector2 vec2Intermediate = vecVec2LongPath.front();

	std::vector<Vector2> vecVec2ShortPath = vecVec2LongPath;
	std::vector<Vector2> vecVec2FinalPath = vecVec2ShortPath;

	PathResult_t result;
	result.eResultType = EPathResultType::FOUND;
	result.vecVec2Path = vecVec2FinalPath;
	return result;
}

StepResult_t NavigationMap::Step(Vector2 vec2CurrPos, std::vector<Vector2> path, float fDist) {
	if(path.empty()) {
		// TODO is this blocked = false?
		return {false, vec2CurrPos};
	}

	Vector2 vec2Move = path.front() - vec2CurrPos;

	if (vec2Move.Length() > fDist) {
		vec2Move = vec2Move.ScaleToLength(fDist);
	}

	Vector2 vec2NewPos = vec2CurrPos + vec2Move;

	if (CompareFloat((vec2NewPos - path.front()).Length(), 0)) {
		std::vector<Vector2> path;
		for(int i = 1; i < path.size(); i++) {
			path.push_back(path.at(i));
		}
		path = path;
	}

	return {false, vec2NewPos};
}


// =============== GRID NAVIGATION ===============


void NavigationMap::Reset() {
}

std::vector<Vector2> NavigationMap::GetLongPath(Vector2 from, Vector2 to) {
	return PlanPath(from, to);
}


Collision_t NavigationMap::CheckEdge(Vector2 vec2Position, Vector2 vec2Destination, std::vector<Vector2>& nodes, std::vector<Node_t>& vecGraphNodes, std::vector<Edge_t>& edges, float collDist) {
	float dist = std::numeric_limits<float>::max();
	Collision_t detected;
	detected.hBlocker = INVALID_HANDLE;
	detected.position = { 0, 0 };

	for (auto& [hBlocker, blocker] : m_blockers) {
		Line line = Line(vec2Position, vec2Destination);
		Circle circle = Circle(Vector2(blocker.x, blocker.y), (collDist + blocker.radius - 1));

		Vector2 coll = TestCollision(line, circle);

		if (coll.Length() > 0 && (coll - vec2Position).Length() < dist){
			dist = (coll - vec2Position).Length();
			detected.position = {blocker.x, blocker.y};
			detected.hBlocker = hBlocker;
		}
	}

	if (detected.hBlocker == INVALID_HANDLE) {
		Edge_t edge1;
		edge1.eType = EEdgeType::LINE;
		edge1.start = vec2Position;
		edge1.end = vec2Destination;
		edges.push_back(edge1);
	} else {
		GridBlocker_t blocker = m_blockers.at(detected.hBlocker);
		Node_t node;
		node.hBlocker = detected.hBlocker;
		node.radius = blocker.radius;
		node.vec2Pos = Vector2(blocker.x, blocker.y);
		AddBlockerToGraph(nodes, edges, vecGraphNodes, node, collDist);
	}

	return detected;
};


void NavigationMap::AddBlockerToGraph(std::vector<Vector2>& nodes, std::vector<Edge_t>& edges, std::vector<Node_t> vecGraphNodes, Node_t nodeToAdd, float collDist) {
	for (Node_t node : vecGraphNodes) {
		if (node.hBlocker == nodeToAdd.hBlocker) {
			return;
		}
	}

	vecGraphNodes.push_back(nodeToAdd);

	for (Node_t node : vecGraphNodes) {
		if (node.hBlocker == nodeToAdd.hBlocker) {
			continue;
		}

		// internal bitangents
		{
			float ra = nodeToAdd.radius;
			float rb = node.radius;
			float d = (nodeToAdd.vec2Pos - node.vec2Pos).Length();
			float a = std::acos((ra+rb)/d);
			Vector2 u = Vector2(node.vec2Pos - nodeToAdd.vec2Pos).Normalize();
			Vector2 v = Vector2(-u.y, u.x);

			Vector2 w1(
				u.x * std::cos(a) - u.y * std::sin(a),
				u.x * std::sin(a) + u.y * std::cos(a)
			);

			Vector2 w2(
				u.x * std::cos(a) + u.y * std::sin(a),
				-u.x * std::sin(a) + u.y * std::cos(a)
			);

			Vector2 C = nodeToAdd.vec2Pos + w1 * ra;
			Vector2 E = nodeToAdd.vec2Pos + w2 * ra;
			Vector2 D = node.vec2Pos - w1 * rb;
			Vector2 F = node.vec2Pos - w2 * rb;

			nodes.push_back(C);
			nodes.push_back(E);
			nodes.push_back(D);
			nodes.push_back(F);

			CheckEdge(C, D, nodes, vecGraphNodes, edges, 0);
			CheckEdge(E, F, nodes, vecGraphNodes, edges, 0);
		}

		// external bitangents
		
		{
			float ra = nodeToAdd.radius;
			float rb = node.radius;
			float d = (node.vec2Pos - nodeToAdd.vec2Pos).Length();
			float a = std::acos((ra-rb)/d);
			Vector2 u = Vector2(node.vec2Pos - nodeToAdd.vec2Pos).Normalize();
			Vector2 v = Vector2(-u.y, u.x);

			Vector2 w1(
				u.x * std::cos(a) - u.y * std::sin(a),
				u.x * std::sin(a) + u.y * std::cos(a)
			);

			Vector2 w2(
				u.x * std::cos(a) + u.y * std::sin(a),
				-u.x * std::sin(a) + u.y * std::cos(a)
			);

			Vector2 C = nodeToAdd.vec2Pos + w1 * ra;
			Vector2 D = node.vec2Pos + w1 * rb;
			Vector2 E = nodeToAdd.vec2Pos + w2 * ra;
			Vector2 F = node.vec2Pos + w2 * rb;

			nodes.push_back(C);
			nodes.push_back(E);
			nodes.push_back(D);
			nodes.push_back(F);

			CheckEdge(C, D, nodes, vecGraphNodes, edges, 0);
			CheckEdge(E, F, nodes, vecGraphNodes, edges, 0);
		}
		
	}
};

#ifdef _DEBUG
DebugPath_t NavigationMap::GetDebugPath(Vector2 from, Vector2 to) {
	DebugPath_t result;

	result.vecVec2LongPath = PlanPath(from, to);

	if (result.vecVec2LongPath.size() < 1) {
		return {};
	}

	Vector2 vec2Intermediate = result.vecVec2LongPath.front();
	
	std::vector<Node_t> vecGraphNodes;
	vecGraphNodes.push_back({
		INVALID_HANDLE,
		from, 
		0
	});
	vecGraphNodes.push_back({
		INVALID_HANDLE,
		to, 
		0
	});


	std::vector<Vector2> nodes;
	std::vector<Edge_t> edges;
	CheckEdge(from, to, nodes, vecGraphNodes, edges, 50);

	std::vector<Vector2> vecVec2ShortPath = { };

	vecVec2ShortPath.push_back(vec2Intermediate);
	result.vecNodes = nodes;
	result.vecEdges = edges;
	result.vecVec2ShortPath = vecVec2ShortPath;

	// std::vector<Vector2> vecVec2SmoothPath = GetSmoothPath(vecVec2ShortPath);
	std::vector<Vector2> vecVec2SmoothPath = vecVec2ShortPath;
	
	for (int i = 1; i < result.vecVec2LongPath.size(); i++) {
		vecVec2SmoothPath.push_back(result.vecVec2LongPath[i]);
	}
	result.vecVec2FinalPath = vecVec2SmoothPath;
	return result;
}
#endif

HGridBlocker NavigationMap::AddBlocker(float x, float y, float radius) {
	GridBlocker_t blocker;
	blocker.x = x;
	blocker.y = y;
	blocker.radius = radius;

	HGridBlocker hBlocker = m_blockers.size();

	m_blockers.emplace(hBlocker, blocker);

	return hBlocker;
}

void NavigationMap::RemoveBlocker(HGridBlocker hBlocker) {
	if (hBlocker == INVALID_HANDLE) {
		Logger::FormatErr("Attempted to remove invalid grid blocker");
		return;
	}

	m_blockers.erase(hBlocker);
}