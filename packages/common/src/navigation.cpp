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

PathResult_t NavigationMap::RequestPath(Vector2 from, Vector2 to, float fCollisionRadius) {
	PathResult_t result;

	std::vector<Vector2> vecVec2LongPath = PlanPath(from, to);

	if (vecVec2LongPath.size() < 1) {
		return {};
	}

	Vector2 vec2Intermediate = vecVec2LongPath.front();

	ShortPathGraph_t graph;
	graph.vecGraphNodes.push_back({INVALID_HANDLE, from, 0});
	graph.vecGraphNodes.push_back({INVALID_HANDLE, vec2Intermediate, 0});

	std::vector<GraphNode_t> vecDiscoveredGraphNodes;
	CheckEdge(from, vec2Intermediate, graph, vecDiscoveredGraphNodes, fCollisionRadius);

	while (!vecDiscoveredGraphNodes.empty()) {
		GraphNode_t node = vecDiscoveredGraphNodes.back();
		vecDiscoveredGraphNodes.pop_back();
		AddBlockerToGraph(graph, node, vecDiscoveredGraphNodes, fCollisionRadius);
	}
	CalculateArcs(graph);

	std::vector<PathSegment_t> vecVec2ShortPath = { };

	auto FindEdge = [](ShortPathGraph_t& graph, int a, int b) -> Edge_t* {
		for (auto& edge : graph.vecEdges) {
			if ((edge.start == a && edge.end == b) || (edge.start == b && edge.end == a)) {
				return &edge;
			}
		}
		return nullptr;
	};

	std::vector<std::vector<std::tuple<int,float, int>>> adjacency = std::vector<std::vector<std::tuple<int,float, int>>>(graph.vecNodes.size());

	for (int e = 0; e < graph.vecEdges.size(); e++) {
		Edge_t edge = graph.vecEdges[e];
		adjacency[edge.start].push_back({edge.end, edge.cost, e});
		adjacency[edge.end].push_back({edge.start, edge.cost, e});
	}

	int start = -1;
	int end = -1;

	for (int i = 0; i < graph.vecNodes.size(); i++) {
		if (graph.vecNodes[i].vec2Pos == from) {
			start = i;
			break;
		}
	}

	for (int i = 0; i < graph.vecNodes.size(); i++) {
		if (graph.vecNodes[i].vec2Pos == vec2Intermediate) {
			end = i;
			break;
		}
	}

	if (graph.vecNodes.size() == 0) {
		PathSegment_t segment;
		segment.eType = EEdgeType::LINE;
		segment.vec2To = vecVec2LongPath[0];
		vecVec2ShortPath.push_back(segment);
		return {EPathResultType::FOUND, vecVec2ShortPath};
	}

	if (start == -1) {
		Logger::Err("Pathing fail: start node not in graph!");
		return {EPathResultType::UNREACHABLE, {}};
	}

	std::vector<float> dist(graph.vecNodes.size(), INFINITY);
	dist[start] = 0;
	std::vector<int> prev(graph.vecNodes.size(), -1);
	std::vector<int> pei(graph.vecNodes.size(), -1);

	std::priority_queue<std::pair<float,int>,std::vector<std::pair<float,int>>,std::greater<>> pq;
	pq.push({0.0f, start});

	std::vector<bool> visited(graph.vecNodes.size(), false);

	while (!pq.empty()) {
		auto [d, u] = pq.top();
		pq.pop();

		if (visited[u]) continue;
		visited[u] = true;

		for (auto& [v, cost, edgeIndex] : adjacency[u]) {
			if (dist[u] + cost < dist[v]) {
				Edge_t edge = graph.vecEdges[edgeIndex];
				dist[v] = dist[u] + cost;
				prev[v] = u;
				pei[v] = edgeIndex;
				pq.push({dist[v], v});
			}
		}
	}

	int i = end;
	while (i != start && i > -1 && i < prev.size()) {
		Edge_t edge = graph.vecEdges[pei[i]];

		if (edge.eType == EEdgeType::LINE) {
			PathSegment_t segment;
			segment.eType = EEdgeType::LINE;
			segment.vec2To = graph.vecNodes[i].vec2Pos;
			vecVec2ShortPath.push_back(segment);
		} else {
			Node_t end = graph.vecNodes[edge.end];
			PathSegment_t segment;
			segment.eType = EEdgeType::ARC;
			segment.vec2To = end.vec2Pos;
			segment.arcCenter = edge.arcCenter;
			segment.arcRadius = edge.arcRadius;
			segment.arcClockwise = edge.arcClockwise;

			/*
			Node_t start = graph.vecNodes[edge.start];
			Node_t end = graph.vecNodes[edge.end];
			float angleStart = ToRadians(CalculateAngle(edge.arcCenter, start.vec2Pos));
			float angleEnd = ToRadians(CalculateAngle(edge.arcCenter, end.vec2Pos));
			
			float canonicalSweep = angleEnd - angleStart;
			if (canonicalSweep < 0) canonicalSweep += 2 * M_PI;

			bool forward = (i == edge.start); // are we walking start->end, or end->start?

			float angleFrom = forward ? angleStart : angleEnd;
			float sweep = forward ? canonicalSweep : -canonicalSweep;



			int steps = 8; // or compute from sagitta formula based on edge.arcRadius
			for (int s = 1; s <= steps; s++) {
				float ang = angleFrom + sweep * (float(s) / steps);
				Vector2 point = edge.arcCenter + Vector2(std::sin(ang), std::cos(ang)) * edge.arcRadius;
				vecVec2ShortPath.push_back(point);
			}
			*/

		}
		i = prev[i];
	}

	if (i != -1) {
		// TODO do something in case i == -1!
		PathSegment_t segment;
		segment.eType = EEdgeType::LINE;
		segment.vec2To = graph.vecNodes[i].vec2Pos;
		vecVec2ShortPath.push_back(segment);
	}

	std::reverse(vecVec2ShortPath.begin(), vecVec2ShortPath.end());

	for (Vector2 vec : vecVec2LongPath) {
		PathSegment_t segment;
		segment.eType = EEdgeType::LINE;
		segment.vec2To = vec;
		vecVec2ShortPath.push_back(segment);
	}

	return {EPathResultType::FOUND, vecVec2ShortPath};
}

StepResult_t NavigationMap::Step(Vector2 vec2CurrPos, std::vector<PathSegment_t> path, float fDist) {
	if(path.empty()) {
		// TODO is this blocked = false?
		return {false, vec2CurrPos};
	}

	Vector2 vec2Move = path.front().vec2To - vec2CurrPos;

	if (vec2Move.Length() > fDist) {
		vec2Move = vec2Move.ScaleToLength(fDist);
	}

	Vector2 vec2NewPos = vec2CurrPos + vec2Move;

	if (CompareFloat((vec2NewPos - path.front().vec2To).Length(), 0)) {
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


Collision_t NavigationMap::CheckEdge(Vector2 vec2Position, Vector2 vec2Destination, ShortPathGraph_t& graph, std::vector<GraphNode_t>& vecDiscoveredGraphNodes, float collDist) {
	float dist = std::numeric_limits<float>::max();
	Collision_t detected;
	detected.hBlocker = INVALID_HANDLE;
	detected.position = { 0, 0 };

	for (auto& [hBlocker, blocker] : m_blockers) {
		Line line = Line(vec2Position, vec2Destination);
		Circle circle = Circle(Vector2(blocker.x, blocker.y), (collDist + blocker.radius - 1));

		Vector2 coll = TestCollision(line, circle);

		if (coll.Length() > 0 && (coll - vec2Position).Length() < (vec2Destination - vec2Position).Length()){
			dist = (coll - vec2Position).Length();
			detected.position = {blocker.x, blocker.y};
			detected.hBlocker = hBlocker;
		}
	}

	if (detected.hBlocker == INVALID_HANDLE) {
	} else {
		GridBlocker_t blocker = m_blockers.at(detected.hBlocker);
		GraphNode_t node;
		node.hBlocker = detected.hBlocker;
		node.radius = blocker.radius + collDist;
		node.vec2Pos = Vector2(blocker.x, blocker.y);
		vecDiscoveredGraphNodes.push_back(node);
	}

	return detected;
};

size_t NavigationMap::AddNodeToGraph(ShortPathGraph_t& graph, HGridBlocker hBlocker, Vector2 vec2Pos) {
	for (int i = 0; i < graph.vecNodes.size(); i++) {
		Node_t node = graph.vecNodes[i];
		if (CompareFloat(node.vec2Pos.x, vec2Pos.x) && CompareFloat(node.vec2Pos.y, vec2Pos.y)) {
			return i;
		}
	}

	graph.vecNodes.push_back({hBlocker, vec2Pos});
	return graph.vecNodes.size() -1;
}

void NavigationMap::AddBlockerToGraph(ShortPathGraph_t& graph, GraphNode_t nodeToAdd, std::vector<GraphNode_t>& vecDiscoveredGraphNodes, float collDist) {
	for (GraphNode_t node : graph.vecGraphNodes) {
		if (node.hBlocker == nodeToAdd.hBlocker) {
			return;
		}
	}

	graph.vecGraphNodes.push_back(nodeToAdd);

	for (GraphNode_t node : graph.vecGraphNodes) {
		if (node.hBlocker == nodeToAdd.hBlocker) {
			continue;
		}

		float ra = nodeToAdd.radius;
		float rb = node.radius;
		float d = (nodeToAdd.vec2Pos - node.vec2Pos).Length();

		// internal bitangents
		if ((ra+rb)/d <= 1 && ra != 0 && rb != 0) {
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

			if (CheckEdge(C, D, graph, vecDiscoveredGraphNodes, collDist).hBlocker == INVALID_HANDLE) {
				Edge_t edge;
				edge.eType = EEdgeType::LINE;
				edge.start = AddNodeToGraph(graph, nodeToAdd.hBlocker, C);
				edge.end = AddNodeToGraph(graph, node.hBlocker, D);
				edge.cost = (C - D).Length();
				graph.vecEdges.push_back(edge);
			}
			if (CheckEdge(E, F, graph, vecDiscoveredGraphNodes, collDist).hBlocker == INVALID_HANDLE) {
				Edge_t edge;
				edge.eType = EEdgeType::LINE;
				edge.start = AddNodeToGraph(graph, nodeToAdd.hBlocker, E);
				edge.end = AddNodeToGraph(graph, node.hBlocker, F);
				edge.cost = (E - F).Length();
				graph.vecEdges.push_back(edge);
			}
		}

		// external bitangents
		if (std::abs(ra-rb)/d <= 1) {
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
			Vector2 E = nodeToAdd.vec2Pos + w2 * ra;
			Vector2 D = node.vec2Pos + w1 * rb;
			Vector2 F = node.vec2Pos + w2 * rb;

			if (CheckEdge(C, D, graph, vecDiscoveredGraphNodes, collDist).hBlocker == INVALID_HANDLE) {
				Edge_t edge;
				edge.eType = EEdgeType::LINE;
				edge.start = AddNodeToGraph(graph, nodeToAdd.hBlocker, C);
				edge.end = AddNodeToGraph(graph, node.hBlocker, D);
				edge.cost = (C - D).Length();
				graph.vecEdges.push_back(edge);
			}
			if (CheckEdge(E, F, graph, vecDiscoveredGraphNodes, collDist).hBlocker == INVALID_HANDLE) {
				Edge_t edge;
				edge.eType = EEdgeType::LINE;
				edge.start = AddNodeToGraph(graph, nodeToAdd.hBlocker, E);
				edge.end = AddNodeToGraph(graph, node.hBlocker, F);
				edge.cost = (E - F).Length();
				graph.vecEdges.push_back(edge);
			}
		} else if (rb == 0) {
			// this is a start point inside the expanded circle!
			Vector2 vec2Escape = nodeToAdd.vec2Pos + (node.vec2Pos - nodeToAdd.vec2Pos).ScaleToLength(ra);
			Edge_t edge;
			edge.eType = EEdgeType::LINE;
			edge.start = AddNodeToGraph(graph, nodeToAdd.hBlocker, vec2Escape);
			edge.end = AddNodeToGraph(graph, node.hBlocker, node.vec2Pos);
			edge.cost = (node.vec2Pos - vec2Escape).Length();
			graph.vecEdges.push_back(edge);
		}
		
	}
};

void NavigationMap::CalculateArcs(ShortPathGraph_t& graph) {
	typedef struct {
		Node_t node;
		int index;
	} NodeWithIndex_t;

	std::list<NodeWithIndex_t> nodesToConsider;

	for (const GraphNode_t& graphNode : graph.vecGraphNodes) {
		nodesToConsider.clear();
		for (auto it = graph.vecNodes.begin(); it != graph.vecNodes.end(); it++) {
			if ((*it).hBlocker != graphNode.hBlocker || (*it).hBlocker == INVALID_HANDLE) {
				continue;
			}

			NodeWithIndex_t nwi;
			nwi.node = *it;
			nwi.index = std::distance(graph.vecNodes.begin(), it);
			nodesToConsider.push_back(nwi);
		}

		nodesToConsider.sort([graphNode](auto a, auto b) {
			float aa = CalculateAngle(a.node.vec2Pos, graphNode.vec2Pos);
			float ab = CalculateAngle(b.node.vec2Pos, graphNode.vec2Pos);
			return aa < ab;
		});

		for (auto it = nodesToConsider.begin(); it != nodesToConsider.end(); it++) {
			auto next = std::next(it, 1);

			if (next == nodesToConsider.end()) {
				next = nodesToConsider.begin();
			}

			float angle1 = ToRadians(CalculateAngle(graphNode.vec2Pos, (*it).node.vec2Pos));
			float angle2 = ToRadians(CalculateAngle(graphNode.vec2Pos, (*next).node.vec2Pos));
			float sweep = angle2 - angle1;
			if (sweep < 0) sweep += 2 * M_PI;  // or use fmod, but watch sign conventions

			Edge_t edge;
			edge.eType = EEdgeType::ARC;
			edge.arcCenter = graphNode.vec2Pos;
			edge.arcRadius = graphNode.radius;
			edge.arcClockwise = true;
			edge.start = (*it).index;
			edge.end = (*next).index;
			edge.cost = graphNode.radius * sweep;
			graph.vecEdges.push_back(edge);
		}
	}
}

#ifdef _DEBUG
DebugPath_t NavigationMap::GetDebugPath(Vector2 from, Vector2 to) {
	DebugPath_t result;

	result.vecVec2LongPath = PlanPath(from, to);

	if (result.vecVec2LongPath.size() < 1) {
		return {};
	}

	Vector2 vec2Intermediate = result.vecVec2LongPath.front();

	ShortPathGraph_t graph;
	graph.vecGraphNodes.push_back({INVALID_HANDLE, from, 0});
	graph.vecGraphNodes.push_back({INVALID_HANDLE, vec2Intermediate, 0});

	std::vector<GraphNode_t> vecDiscoveredGraphNodes;
	CheckEdge(from, vec2Intermediate, graph, vecDiscoveredGraphNodes, 50);

	while (!vecDiscoveredGraphNodes.empty()) {
		GraphNode_t node = vecDiscoveredGraphNodes.back();
		vecDiscoveredGraphNodes.pop_back();
		AddBlockerToGraph(graph, node, vecDiscoveredGraphNodes, 50);
	}
	CalculateArcs(graph);

	std::vector<PathSegment_t> vecVec2ShortPath = { };
	{
		PathSegment_t segment;
		segment.eType = EEdgeType::LINE;
		segment.vec2To = vec2Intermediate;
		vecVec2ShortPath.push_back(segment);
	}
	result.vecVec2ShortPath = vecVec2ShortPath;

	std::vector<PathSegment_t> vecVec2SmoothPath = vecVec2ShortPath;
	for (int i = 1; i < result.vecVec2LongPath.size(); i++) {
		PathSegment_t segment;
		segment.eType = EEdgeType::LINE;
		segment.vec2To = result.vecVec2LongPath[i];
		vecVec2SmoothPath.push_back(segment);
	}
	result.vecEdges = graph.vecEdges;
	result.vecNodes = graph.vecNodes;
	result.vecVec2ShortPath = vecVec2ShortPath;
	result.vecVec2FinalPath = vecVec2SmoothPath;

	auto FindEdge = [](ShortPathGraph_t& graph, int a, int b) -> Edge_t* {
		for (auto& edge : graph.vecEdges) {
			if ((edge.start == a && edge.end == b) || (edge.start == b && edge.end == a)) {
				return &edge;
			}
		}
		return nullptr;
	};

	std::vector<std::vector<std::tuple<int,float, int>>> adjacency = std::vector<std::vector<std::tuple<int,float, int>>>(graph.vecNodes.size());

	for (int e = 0; e < graph.vecEdges.size(); e++) {
		Edge_t edge = graph.vecEdges[e];
		adjacency[edge.start].push_back({edge.end, edge.cost, e});
		adjacency[edge.end].push_back({edge.start, edge.cost, e});
	}

	int start = -1;
	int end = -1;

	for (int i = 0; i < graph.vecNodes.size(); i++) {
		if (graph.vecNodes[i].vec2Pos == from) {
			start = i;
			break;
		}
	}

	for (int i = 0; i < graph.vecNodes.size(); i++) {
		if (graph.vecNodes[i].vec2Pos == vec2Intermediate) {
			end = i;
			break;
		}
	}

	if (graph.vecNodes.size() == 0) {
		PathSegment_t segment;
		segment.eType = EEdgeType::LINE;
		segment.vec2To = result.vecVec2LongPath[0];
		result.vecVec2ShortPath.push_back(segment);
		return result;
	}

	std::vector<float> dist(graph.vecNodes.size(), INFINITY);
	dist[start] = 0;
	std::vector<int> prev(graph.vecNodes.size(), -1);
	std::vector<int> pei(graph.vecNodes.size(), -1);

	std::priority_queue<std::pair<float,int>,std::vector<std::pair<float,int>>,std::greater<>> pq;
	pq.push({0.0f, start});

	std::vector<bool> visited(graph.vecNodes.size(), false);

	while (!pq.empty()) {
		auto [d, u] = pq.top();
		pq.pop();

		if (visited[u]) continue;
		visited[u] = true;

		for (auto& [v, cost, edgeIndex] : adjacency[u]) {
			if (dist[u] + cost < dist[v]) {
				Edge_t edge = graph.vecEdges[edgeIndex];
				dist[v] = dist[u] + cost;
				prev[v] = u;
				pei[v] = edgeIndex;
				pq.push({dist[v], v});
			}
		}
	}

	int i = end;
	while (i != start && i > -1 && i < prev.size()) {
		Edge_t edge = graph.vecEdges[pei[i]];

		if (edge.eType == EEdgeType::LINE) {
			PathSegment_t segment;
			segment.eType = EEdgeType::LINE;
			segment.vec2To = graph.vecNodes[i].vec2Pos;
			result.vecVec2ShortPath.push_back(segment);
		} else {
			Node_t end = graph.vecNodes[edge.end];
			Node_t start = graph.vecNodes[edge.start];
			PathSegment_t segment;
			segment.eType = EEdgeType::ARC;
			segment.vec2To = start.vec2Pos;
			segment.arcCenter = edge.arcCenter;
			segment.arcRadius = edge.arcRadius;
			segment.arcClockwise = edge.arcClockwise;
			result.vecVec2ShortPath.push_back(segment);
			/*
			Node_t start = graph.vecNodes[edge.start];
			Node_t end = graph.vecNodes[edge.end];
			float angleStart = ToRadians(CalculateAngle(edge.arcCenter, start.vec2Pos));
			float angleEnd = ToRadians(CalculateAngle(edge.arcCenter, end.vec2Pos));
			
			float canonicalSweep = angleEnd - angleStart;
			if (canonicalSweep < 0) canonicalSweep += 2 * M_PI;

			bool forward = (i == edge.start); // are we walking start->end, or end->start?

			float angleFrom = forward ? angleStart : angleEnd;
			float sweep = forward ? canonicalSweep : -canonicalSweep;



			int steps = 8; // or compute from sagitta formula based on edge.arcRadius
			for (int s = 1; s <= steps; s++) {
				float ang = angleFrom + sweep * (float(s) / steps);
				Vector2 point = edge.arcCenter + Vector2(std::sin(ang), std::cos(ang)) * edge.arcRadius;
				result.vecVec2ShortPath.push_back(point);
			}
			*/

		}
		i = prev[i];
	}

	if (i != -1) {
		// TODO do something in case i == -1!
		PathSegment_t segment;
		segment.eType = EEdgeType::LINE;
		segment.vec2To = graph.vecNodes[i].vec2Pos;
		result.vecVec2ShortPath.push_back(segment);
	}

	std::reverse(result.vecVec2ShortPath.begin(), result.vecVec2ShortPath.end());
	result.vecVec2FinalPath = result.vecVec2ShortPath;

	return result;
}
#endif

HGridBlocker NavigationMap::AddBlocker(float x, float y, float radius) {
	GridBlocker_t blocker;
	blocker.x = x;
	blocker.y = y;
	blocker.radius = radius;

	HGridBlocker hBlocker = m_currBlockerIndex++;

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

Vector2 NavigationMap::ClosestFreePosition(Vector2 vec2Target) {
	return vec2Target;
}