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
				tunnelRight = {from->right.x, from->left.z};
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

Paths_t NavigationMap::GetDebugPath(NavigationGridAgent* pAgent, Vector2 from, Vector2 to) {
	to = GetClosestFreeSpot(pAgent, from, to);

	if (from == to) {
		return {};
	}

	std::vector<Vector2> vecLongPath = PlanPath(pAgent, { from.x, from.y }, { to.x, to.y });

	std::vector<Vector2> vecShortPath = RefinePath(pAgent, vecLongPath, from, to);

	std::vector<Vector2> vec2ShortPathSmoothed = SmoothPath(pAgent, vecShortPath, from, to);
	
	std::vector<Vector2> vecVec2FinalPath = vec2ShortPathSmoothed;
	
	for (Vector2 vec : vecLongPath) {
		vecVec2FinalPath.push_back(vec);
	}

	Paths_t paths;
	paths.vecVec2FinalPath = vecVec2FinalPath;
	paths.vecVec2LongPath = vecLongPath;
	paths.vecVec2ShortPath = vecShortPath;

	return paths;
}

std::vector<Vector2> NavigationMap::SmoothPath(NavigationGridAgent* pAgent, const std::vector<Vector2>& vecShortPath, Vector2 start, Vector2 end) {
	std::vector<Vector2> vec2ShortPathSmoothed = {};
	Vector2 vec2Anchor = start;
	Vector2 vec2Previous = start;

	
	struct Collision_t {
		NavigationGridAgent* pAgent;
		Vector2 position;
	};

	auto GetObstruction = [this, pAgent, end](Vector2 vec2Position, Vector2 vec2Destination, NavigationGridAgent* pIgnoreAgent, float collDist) -> Collision_t {
		NavigationGridAgent* pCollAg= nullptr;
		float dist = std::numeric_limits<float>::max();
		struct Collision_t detected;
		detected.pAgent = nullptr;
		detected.position = { 0, 0 };

		for (NavigationGridAgent* pOtherAgent : m_vecAgents) {
			if (pOtherAgent == pIgnoreAgent || pOtherAgent == pAgent) {
				continue;
			}

			if (!pOtherAgent->path.empty()) {
				continue;
			}

			Line line = Line(vec2Position, vec2Destination);
			Circle circle = Circle(pOtherAgent->position, (collDist + pOtherAgent->nCollisionRadius));

			Vector2 coll = TestCollision(line, circle);

			if (coll.Length() > 0 && (coll - vec2Position).Length() < dist){
				dist = (coll - vec2Position).Length();
				detected.position = coll;
				detected.pAgent = pOtherAgent;
			}
		}
		return detected;
	};


	for(int i = 0; i < vecShortPath.size(); ) {
		vec2Anchor = vecShortPath.at(i);
		int next = i + 1;

		for(int j = next; j < vecShortPath.size(); j++) {
			Vector2 vec = vecShortPath.at(j);
			Collision_t coll = GetObstruction(vec2Anchor, vec, nullptr, pAgent->nCollisionRadius);
			if(coll.pAgent == nullptr) {
				next = j;
			}
		}

		if(i == vecShortPath.size() - 1) {
			break;
		}
		vec2ShortPathSmoothed.push_back(vecShortPath.at(next));
		if(i == next) {
			break;
		}

		i = next;
	}

	return vec2ShortPathSmoothed;
}

std::vector<Vector2> NavigationMap::RefinePath(NavigationGridAgent* pAgent, const std::vector<Vector2>& vecLongPath, Vector2 from, Vector2 to) {
	if (vecLongPath.empty()) {
		return {};
	}

	Vector2 start = from;
	Vector2 end = vecLongPath.at(0);
	std::vector<Vector2> vecShortPath = { };

	if (start == end) {
		return {};
	}

	struct Collision_t {
		NavigationGridAgent* pAgent;
		Vector2 position;
	};

	auto GetObstruction = [this, pAgent, end](Vector2 vec2Position, Vector2 vec2Destination, NavigationGridAgent* pIgnoreAgent, float collDist) -> Collision_t {
		NavigationGridAgent* pCollAg= nullptr;
		float dist = std::numeric_limits<float>::max();
		struct Collision_t detected;
		detected.pAgent = nullptr;
		detected.position = { 0, 0 };

		for (NavigationGridAgent* pOtherAgent : m_vecAgents) {
			if (pOtherAgent == pIgnoreAgent || pOtherAgent == pAgent) {
				continue;
			}

			if (!pOtherAgent->path.empty()) {
				continue;
			}

			Line line = Line(vec2Position, vec2Destination);
			Circle circle = Circle(pOtherAgent->position, (collDist + pOtherAgent->nCollisionRadius));

			Vector2 coll = TestCollision(line, circle);

			if (coll.Length() > 0 && (coll - vec2Position).Length() < dist){
				dist = (coll - vec2Position).Length();
				detected.position = coll;
				detected.pAgent = pOtherAgent;
			}
		}
		return detected;
	};

	

	int nMaxIter = 10000;
	int nIter = 0;
	
	struct Trace_t {
		Collision_t collision;
		NavigationGridAgent* pTracing;
		Vector2 vec2Position;
		float fTotalDist;
		std::vector<Vector2> vecPath;
		Vector2 vec2InitialColl;
	};

	Trace_t cw { .pTracing = nullptr, .vec2Position = start, .fTotalDist = 0.0f, };
	Trace_t ccw { .pTracing = nullptr, .vec2Position = start, .fTotalDist = 0.0f };

	cw.collision = GetObstruction(cw.vec2Position, end, nullptr, pAgent->nCollisionRadius);
	ccw.collision = GetObstruction(ccw.vec2Position, end, nullptr, pAgent->nCollisionRadius);
	cw.vecPath.push_back(start);
	ccw.vecPath.push_back(start);

	if (cw.collision.pAgent != nullptr && cw.collision.position != start) {
		cw.vec2Position = cw.collision.position;
		cw.vec2InitialColl = cw.collision.position;
		cw.pTracing = cw.collision.pAgent;
		cw.vecPath.push_back(cw.vec2Position);
	}

	if (ccw.collision.pAgent != nullptr && ccw.collision.position != start) {
		ccw.vec2Position = ccw.collision.position;
		ccw.vec2InitialColl = ccw.collision.position;
		ccw.pTracing = ccw.collision.pAgent;
		ccw.vecPath.push_back(ccw.vec2Position);
	}

#define STEP_SIZE 50
	while (cw.vec2Position != end && ccw.vec2Position != end) {
		// only trace one direction each time
		bool traceCW = cw.fTotalDist <= ccw.fTotalDist;
		Trace_t& trace = traceCW ? cw : ccw;

		// tracing
		if (trace.pTracing != nullptr) {
			// find next tangent to traced target
			Vector2 vec2CircleCenterToAgent = trace.pTracing->position - trace.vec2Position;
			Vector2 vec2Tangent = traceCW ? Vector2(-vec2CircleCenterToAgent.y, vec2CircleCenterToAgent.x)
				: Vector2(vec2CircleCenterToAgent.y, -vec2CircleCenterToAgent.x);
			vec2Tangent = vec2Tangent.ScaleToLength(STEP_SIZE);
			Vector2 next = trace.vec2Position + vec2Tangent;
			
			// check if tangent is free
			Collision_t newCollision = GetObstruction(trace.vec2Position, next, trace.collision.pAgent, pAgent->nCollisionRadius);
			
			// check if we're back on target
			Line line;
			line.Start = start;
			line.End = end;
			Line otherLine;
			otherLine.Start = trace.vec2Position;
			otherLine.End = next;
			std::optional<Vector2> vec2GoalLineIntersect = TestCollision(line, otherLine);
			bool bIsValidIntersect = vec2GoalLineIntersect.has_value()
				&& (end - vec2GoalLineIntersect.value()).Length() - (end - trace.vec2InitialColl).Length() < -1;

			

			// crossing the target line but also running into something
			if(bIsValidIntersect && newCollision.pAgent != nullptr) {
				if((vec2GoalLineIntersect.value() - trace.vec2Position).Length() < (newCollision.position - trace.vec2Position).Length()) {
					// back on track?
					trace.pTracing = nullptr;
					trace.vecPath.push_back(vec2GoalLineIntersect.value());
					trace.fTotalDist += (vec2GoalLineIntersect.value() - trace.vec2Position).Length();
					trace.vec2Position = vec2GoalLineIntersect.value();
				} else {
					trace.pTracing = newCollision.pAgent;
					trace.collision = newCollision;
					trace.fTotalDist += (newCollision.position - trace.vec2Position).Length();
					trace.vec2Position = newCollision.position;
					trace.vecPath.push_back(trace.vec2Position);
				}
			// running into something
			} else if (newCollision.pAgent != nullptr) {
				trace.pTracing = newCollision.pAgent;
				trace.collision = newCollision;
				trace.fTotalDist += (newCollision.position - trace.vec2Position).Length();
				trace.vec2Position = newCollision.position;
				trace.vecPath.push_back(trace.vec2Position);
			// back on target line
			} else if (bIsValidIntersect) {
				// back on track?
				trace.pTracing = nullptr;
				trace.vecPath.push_back(vec2GoalLineIntersect.value());
				trace.fTotalDist += (vec2GoalLineIntersect.value() - trace.vec2Position).Length();
				trace.vec2Position = vec2GoalLineIntersect.value();
			// keep going tangent
			} else {
				trace.fTotalDist += (next - trace.vec2Position).Length();
				trace.vec2Position = next;
				trace.vecPath.push_back(trace.vec2Position);
			}

		}
		// not tracing
		else if(trace.pTracing == nullptr) {
			Collision_t newCollision = GetObstruction(trace.vec2Position, end, nullptr, pAgent->nCollisionRadius);
			if(newCollision.pAgent) {
				trace.fTotalDist += (newCollision.position - trace.vec2Position).Length();
				trace.vec2Position = newCollision.position;
				trace.pTracing = newCollision.pAgent;
				trace.collision = newCollision;
				trace.vec2InitialColl = trace.collision.position;
				trace.vecPath.push_back(trace.vec2Position);
			} else {
				// not tracing & no obstruction - done?
				trace.fTotalDist += (end - trace.vec2Position).Length();
				trace.vec2Position = end;
				trace.vecPath.push_back(end);
			}
		}

		nIter++;
		if (nIter >= nMaxIter) {
			break;
		}
	}

	if(cw.vec2Position == end) {
		vecShortPath = cw.vecPath;
	} else {
		vecShortPath = ccw.vecPath;
	}

	return vecShortPath;
}
std::vector<Vector2> NavigationMap::GetPath(NavigationGridAgent* pAgent, Vector2 from, Vector2 to) {
	to = GetClosestFreeSpot(pAgent, from, to);

	if (from == to) {
		return {};
	}

	
	std::vector<Vector2> vecLongPath = PlanPath(pAgent, { from.x, from.y }, { to.x, to.y });

	std::vector<Vector2> vecShortPath = RefinePath(pAgent, vecLongPath, from, to);

	std::vector<Vector2> vec2ShortPathSmoothed = SmoothPath(pAgent, vecShortPath, from, to);
	
	for (Vector2 vec : vecLongPath) {
		vec2ShortPathSmoothed.push_back(vec);
	}

	return vec2ShortPathSmoothed;
}

StepResult_t NavigationMap::Step(NavigationGridAgent* pAgent, Vector2 vec2CurrPos, float fDist) {
	if(pAgent->path.empty()) {
		// TODO is this blocked = false?
		return {false, vec2CurrPos};
	}

	Vector2 vec2Move = pAgent->path.front() - vec2CurrPos;

	if (vec2Move.Length() > fDist) {
		vec2Move = vec2Move.ScaleToLength(fDist);
	}

	Vector2 vec2NewPos = vec2CurrPos + vec2Move;

	for (NavigationGridAgent* pOtherAgent : m_vecAgents) {
		if (pAgent->UnitId == pOtherAgent->UnitId) {{
			continue;
		}}

		if ((pAgent->position - pOtherAgent->position).Length() <= pOtherAgent->nCollisionRadius
				&& (vec2NewPos - pOtherAgent->position).Length() < (pAgent->position - pOtherAgent->position).Length()) {
			return {true, vec2CurrPos};
		}
	}

	if (CompareFloat((vec2NewPos - pAgent->path.front()).Length(), 0)) {
		std::vector<Vector2> path;
		for(int i = 1; i < pAgent->path.size(); i++) {
			path.push_back(pAgent->path.at(i));
		}
		pAgent->path = path;
	}

	return {false, vec2NewPos};
}

NavigationGridAgent* NavigationMap::CreateAgent() {
	NavigationGridAgent* pAgent = new NavigationGridAgent();
	m_vecAgents.push_back(pAgent);
	return pAgent;
}

void NavigationMap::Reset() {
}

std::vector<Vector2> NavigationMap::GetCoarseGridPath(NavigationGridAgent* pAgent, Vector2 from, Vector2 to) {	
	// TODO properly do this, find closest spot inside navmesh as well
	for(NavigationGridAgent* pOther : m_vecAgents) {
		if(pOther->UnitId == pAgent->UnitId) {
			continue;
		}
		if((pOther->position - to).Length() <= pOther->nCollisionRadius) {
			// destination currently blocked
			Logger::FormatMsg("Destination for %d blocked, picking closest available point", pAgent->UnitId);
			to = pOther->position + (from - pOther->position).ScaleToLength(((pOther->nCollisionRadius / 2) + (pAgent->nCollisionRadius / 2)));
		}
	}

	if (from == to) {
		return {};
	}

	std::vector<Vector2> vecLongPath = PlanPath(pAgent, { from.x, from.y }, { to.x, to.y });
	Vector2 start = from;
	Vector2 end = vecLongPath.at(0);
	std::vector<Vector2> vecShortPath = { };

	if (start == end) {
		return {};
	}

	struct Collision_t {
		NavigationGridAgent* pAgent;
		Vector2 position;
	};

	auto GetObstruction = [this, pAgent, end](Vector2 vec2Position, Vector2 vec2Destination, NavigationGridAgent* pIgnoreAgent, float collDist) -> Collision_t {
		NavigationGridAgent* pCollAg= nullptr;
		float dist = std::numeric_limits<float>::max();
		struct Collision_t detected;
		detected.pAgent = nullptr;
		detected.position = { 0, 0 };

		for (NavigationGridAgent* pOtherAgent : m_vecAgents) {
			if (pOtherAgent == pIgnoreAgent || pOtherAgent == pAgent) {
				continue;
			}

			if (!pOtherAgent->path.empty()) {
				continue;
			}

			Line line = Line(vec2Position, vec2Destination);
			Circle circle = Circle(pOtherAgent->position, (collDist + pOtherAgent->nCollisionRadius));

			Vector2 coll = TestCollision(line, circle);

			if (coll.Length() > 0 && (coll - vec2Position).Length() < dist){
				dist = (coll - vec2Position).Length();
				detected.position = coll;
				detected.pAgent = pOtherAgent;
			}
		}
		return detected;
	};

	

	int nMaxIter = 10000;
	int nIter = 0;
	
	struct Trace_t {
		Collision_t collision;
		NavigationGridAgent* pTracing;
		Vector2 vec2Position;
		float fTotalDist;
		std::vector<Vector2> vecPath;
		Vector2 vec2InitialColl;
	};

	Trace_t cw { .pTracing = nullptr, .vec2Position = start, .fTotalDist = 0.0f, };
	Trace_t ccw { .pTracing = nullptr, .vec2Position = start, .fTotalDist = 0.0f };

	cw.collision = GetObstruction(cw.vec2Position, end, nullptr, pAgent->nCollisionRadius);
	ccw.collision = GetObstruction(ccw.vec2Position, end, nullptr, pAgent->nCollisionRadius);
	cw.vecPath.push_back(start);
	ccw.vecPath.push_back(start);

	if (cw.collision.pAgent != nullptr && cw.collision.position != start) {
		cw.vec2Position = cw.collision.position;
		cw.vec2InitialColl = cw.collision.position;
		cw.pTracing = cw.collision.pAgent;
		cw.vecPath.push_back(cw.vec2Position);
	}

	if (ccw.collision.pAgent != nullptr && ccw.collision.position != start) {
		ccw.vec2Position = ccw.collision.position;
		ccw.vec2InitialColl = ccw.collision.position;
		ccw.pTracing = ccw.collision.pAgent;
		ccw.vecPath.push_back(ccw.vec2Position);
	}

#define STEP_SIZE 50
	while (cw.vec2Position != end && ccw.vec2Position != end) {
		// only trace one direction each time
		bool traceCW = cw.fTotalDist <= ccw.fTotalDist;
		Trace_t& trace = traceCW ? cw : ccw;

		// tracing
		if (trace.pTracing != nullptr) {
			// find next tangent to traced target
			Vector2 vec2CircleCenterToAgent = trace.pTracing->position - trace.vec2Position;
			Vector2 vec2Tangent = traceCW ? Vector2(-vec2CircleCenterToAgent.y, vec2CircleCenterToAgent.x)
				: Vector2(vec2CircleCenterToAgent.y, -vec2CircleCenterToAgent.x);
			vec2Tangent = vec2Tangent.ScaleToLength(STEP_SIZE);
			Vector2 next = trace.vec2Position + vec2Tangent;
			
			// check if tangent is free
			Collision_t newCollision = GetObstruction(trace.vec2Position, next, trace.collision.pAgent, pAgent->nCollisionRadius);
			
			// check if we're back on target
			Line line;
			line.Start = start;
			line.End = end;
			Line otherLine;
			otherLine.Start = trace.vec2Position;
			otherLine.End = next;
			std::optional<Vector2> vec2GoalLineIntersect = TestCollision(line, otherLine);
			bool bIsValidIntersect = vec2GoalLineIntersect.has_value()
				&& (end - vec2GoalLineIntersect.value()).Length() - (end - trace.vec2InitialColl).Length() < -1;

			

			// crossing the target line but also running into something
			if(bIsValidIntersect && newCollision.pAgent != nullptr) {
				if((vec2GoalLineIntersect.value() - trace.vec2Position).Length() < (newCollision.position - trace.vec2Position).Length()) {
					// back on track?
					trace.pTracing = nullptr;
					trace.vecPath.push_back(vec2GoalLineIntersect.value());
					trace.fTotalDist += (vec2GoalLineIntersect.value() - trace.vec2Position).Length();
					trace.vec2Position = vec2GoalLineIntersect.value();
				} else {
					trace.pTracing = newCollision.pAgent;
					trace.collision = newCollision;
					trace.fTotalDist += (newCollision.position - trace.vec2Position).Length();
					trace.vec2Position = newCollision.position;
					trace.vecPath.push_back(trace.vec2Position);
				}
			// running into something
			} else if (newCollision.pAgent != nullptr) {
				trace.pTracing = newCollision.pAgent;
				trace.collision = newCollision;
				trace.fTotalDist += (newCollision.position - trace.vec2Position).Length();
				trace.vec2Position = newCollision.position;
				trace.vecPath.push_back(trace.vec2Position);
			// back on target line
			} else if (bIsValidIntersect) {
				// back on track?
				trace.pTracing = nullptr;
				trace.vecPath.push_back(vec2GoalLineIntersect.value());
				trace.fTotalDist += (vec2GoalLineIntersect.value() - trace.vec2Position).Length();
				trace.vec2Position = vec2GoalLineIntersect.value();
			// keep going tangent
			} else {
				trace.fTotalDist += (next - trace.vec2Position).Length();
				trace.vec2Position = next;
				trace.vecPath.push_back(trace.vec2Position);
			}

		}
		// not tracing
		else if(trace.pTracing == nullptr) {
			Collision_t newCollision = GetObstruction(trace.vec2Position, end, nullptr, pAgent->nCollisionRadius);
			if(newCollision.pAgent) {
				trace.fTotalDist += (newCollision.position - trace.vec2Position).Length();
				trace.vec2Position = newCollision.position;
				trace.pTracing = newCollision.pAgent;
				trace.collision = newCollision;
				trace.vec2InitialColl = trace.collision.position;
				trace.vecPath.push_back(trace.vec2Position);
			} else {
				// not tracing & no obstruction - done?
				trace.fTotalDist += (end - trace.vec2Position).Length();
				trace.vec2Position = end;
				trace.vecPath.push_back(end);
			}
		}

		nIter++;
		if (nIter >= nMaxIter) {
			break;
		}
	}

	if(cw.vec2Position == end) {
		vecShortPath = cw.vecPath;
	} else {
		vecShortPath = ccw.vecPath;
	}

	std::vector<Vector2> vec2ShortPathSmoothed = {};
	Vector2 vec2Anchor = start;
	Vector2 vec2Previous = start;
	for(int i = 0; i < vecShortPath.size(); i++) {
		vec2ShortPathSmoothed.push_back(vecShortPath.at(i));
	}

	for (Vector2 vec : vecLongPath) {
		vec2ShortPathSmoothed.push_back(vec);
	}

	Logger::FormatMsg("Unit %d going from %f, %f to %f, %f via %d cells", pAgent->UnitId, from.x, from.y, to.x, to.y, vec2ShortPathSmoothed.size());

	return vec2ShortPathSmoothed;
}


Vector2 NavigationMap::GetClosestFreeSpot(NavigationGridAgent* pAgent, Vector2 vec2From, Vector2 vec2Original) {
	// TODO properly do this, find closest spot inside navmesh as well
	for(NavigationGridAgent* pOther : m_vecAgents) {
		if(pOther->UnitId == pAgent->UnitId) {
			continue;
		}
		if((pOther->position - vec2Original).Length() <= pOther->nCollisionRadius + pAgent->nCollisionRadius) {
			// destination currently blocked
			Logger::FormatMsg("Destination for %d blocked, picking closest available point", pAgent->UnitId);
			Vector2 vec2TargetOffsetFromOther = vec2Original - pOther->position;

			if (vec2TargetOffsetFromOther.Length() == 0) {
				vec2TargetOffsetFromOther = (vec2From - pOther->position).Normalize().ScaleToLength(pOther->nCollisionRadius + pAgent->nCollisionRadius + 10);
			}
			vec2Original = pOther->position + vec2TargetOffsetFromOther.Normalize().ScaleToLength(pOther->nCollisionRadius + pAgent->nCollisionRadius + 10);
			return GetClosestFreeSpot(pAgent, vec2From, vec2Original);
		}
	}

	return vec2Original;
}