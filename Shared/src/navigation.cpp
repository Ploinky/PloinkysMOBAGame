#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <limits>
#include <list>
#include "navigation.h"
#include "util.h"
#include <fstream>


namespace PMG {
	bool operator==(const vertex_t& lhs, const vertex_t& rhs) {
		return lhs.x == rhs.x && lhs.y == rhs.y && lhs.z == rhs.z;
	}

	vertex_t operator-(const vertex_t& lhs, const vertex_t& rhs) {
		return { lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z };
	}

	vertex_t CenterOf(vertex_t v1, vertex_t v2, vertex_t v3) {
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
		from = { 0, 0, 0 };
		to = { 0, 0, 0 };
	}

	polygon_t* NavMesh::CreatePolygon(vertex_t v1, vertex_t v2, vertex_t v3) {
		polygon_t* p = new polygon_t;
		p->id = polygonId++;
		p->center = CenterOf(v1, v2, v3);
		p->vertices[0] = v1;
		p->vertices[1] = v2;
		p->vertices[2] = v3;

		return p;
	}

	float NavMesh::Sign(vertex_t p1, vertex_t p2, vertex_t p3) {
		return (p1.x - p3.x) * (p2.z - p3.z) - (p2.x - p3.x) * (p1.z - p3.z);
	}

	bool NavMesh::PointInTriangle(vertex_t pt, vertex_t v1, vertex_t v2, vertex_t v3) {
		float d1, d2, d3;
		bool has_neg, has_pos;

		d1 = Sign(pt, v1, v2);
		d2 = Sign(pt, v2, v3);
		d3 = Sign(pt, v3, v1);

		has_neg = (d1 < 0) || (d2 < 0) || (d3 < 0);
		has_pos = (d1 > 0) || (d2 > 0) || (d3 > 0);

		return !(has_neg && has_pos);
	}

	bool NavMesh::PointInMesh(vertex_t pt) {
		for (polygon_t* poly : mesh) {
			if (PointInTriangle(pt, poly->vertices[0], poly->vertices[1], poly->vertices[2])) {
				return true;
			}
		}

		return false;
	}

	polygon_t* NavMesh::FindPolygonAt(vertex_t pt) {
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

	float NavMesh::Distance(vertex_t a, vertex_t b) {
		return abs(a.x - b.x) + abs(a.y - b.y) + abs(a.z - b.z);
	}

	float NavMesh::Distance(polygon_t* a, polygon_t* b) {
		return Distance(a->center, b->center);
	}

	float NavMesh::Cross(const vertex_t v1, const vertex_t v2) {
		return v1.x * v2.z - v1.z * v2.x;
	}

	float NavMesh::AngleBetween(vertex_t a, vertex_t b) {
		return atan2(b.z - a.z, b.x - a.x);
	}

	void NavMesh::PullString() {

		movePath.clear();

		if (FindPolygonAt(from) == FindPolygonAt(to)) {
			movePath.push_back(from);
			movePath.push_back(to);
			return;
		}

		std::list<vertex_t> leftVertices;
		std::list<vertex_t> rightVertices;

		vertex_t apex = path.front()->center;

		std::list<portal_t> portals;

		for (auto poly = path.begin(); poly != path.end(); poly++)
		{
			if (poly == std::prev(path.end())) {
				continue;
			}
			portal_t* p = new portal_t;
			p->from = *poly;
			p->to = *std::next(poly);

			vertex_t vf1 = p->from->vertices[0];
			vertex_t vf2 = p->from->vertices[1];
			vertex_t vf3 = p->from->vertices[2];

			vertex_t vt1 = p->to->vertices[0];
			vertex_t vt2 = p->to->vertices[1];
			vertex_t vt3 = p->to->vertices[2];

			vertex_t av1 = { 0 };
			vertex_t av2 = { 0 };

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

		vertex_t currVert = from;
		movePath.push_back(currVert);

		auto leftFrom = portals.begin();
		auto rightFrom = portals.begin();

		auto from = portals.begin();
		//std::cout << "========================================================" << std::endl;
		while (!(currVert == to)) {
			//std::cout << "Apex: " << currVert.x << ", " << currVert.y << std::endl;

			vertex_t tunnelLeft = from->left;
			vertex_t tunnelRight = from->right;

			for (auto portal = from; portal != portals.end(); portal++) {
				//std::cout << "Portal: " << portal->from->id << " -> " << portal->to->id << std::endl;
				vertex_t rightTo = portal->right;
				vertex_t leftTo = portal->left;

				float rightNext = Cross(tunnelRight - currVert, rightTo - currVert);
				float rightCross = Cross(rightTo - currVert, tunnelLeft - currVert);

				if (rightCross > 0) {
					// Left is new Apex!
					currVert = tunnelLeft;
					tunnelLeft = from->left;
					tunnelRight = from->right;
					from = leftFrom;
					movePath.push_back(currVert);
					//std::cout << "\tNew apex: " << currVert.x << ", " << currVert.y << std::endl;
					break;
				}
				else if (rightNext <= 0) {
					// Right is new funnel right!
					tunnelRight = rightTo;
					rightFrom = portal;
					//std::cout << "\tRight: " << tunnelRight.x << ", " << tunnelRight.y << std::endl;
				}
				// if tunnel does not tighten, we have a new apex!
				// if we're gonna cross over, do not advance!
				float leftNext = Cross(tunnelLeft - currVert, leftTo - currVert);
				float leftCross = Cross(leftTo - currVert, tunnelRight - currVert);

				if (leftCross < 0) {
					// Right is new Apex!
					currVert = tunnelRight;
					tunnelLeft = from->left;
					tunnelRight = from->right;
					from = rightFrom;
					movePath.push_back(currVert);
					//std::cout << "\tNew apex: " << currVert.x << ", " << currVert.y << std::endl;
					break;
				}
				else if (leftNext >= 0) {
					// Left is new funnel left!
					tunnelLeft = leftTo;
					leftFrom = portal;
					//std::cout << "\tLeft: " << tunnelLeft.x << ", " << tunnelLeft.y << std::endl;
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
						//std::cout << "\tNew apex: " << currVert.x << ", " << currVert.y << std::endl;
						break;
					}
					else if (rightCross == 0) {
						currVert = to;
						movePath.push_back(currVert);
						//std::cout << "\tNew apex: " << currVert.x << ", " << currVert.y << std::endl;
						break;
					}
					else if (rightNext <= 0) {
						// Right is new funnel right!
						tunnelRight = rightTo;
						rightFrom = portal;
						//std::cout << "\tRight: " << tunnelRight.x << ", " << tunnelRight.y << std::endl;
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
						//std::cout << "\tNew apex: " << currVert.x << ", " << currVert.y << std::endl;
						break;
					}
					else if (leftCross == 0) {
						currVert = to;
						movePath.push_back(currVert);
						//std::cout << "\tNew apex: " << currVert.x << ", " << currVert.y << std::endl;
						break;

					}
					else if (leftNext >= 0) {
						// Left is new funnel left!
						tunnelLeft = leftTo;
						leftFrom = portal;
						//std::cout << "\tLeft: " << tunnelLeft.x << ", " << tunnelLeft.y << std::endl;
					}
				}
			}
		}
	}

	std::list<vertex_t> NavMesh::PlanPath(vertex_t from, vertex_t to) {
		this->from = from;
		this->to = to;

		polygon_t* startPoly = FindPolygonAt(from);
		polygon_t* endPoly = FindPolygonAt(to);

		if (startPoly == nullptr || endPoly == nullptr) {
			// What to do when click is not in nav mesh?
			return {};
		}

		for (auto pol : mesh) {
			pol->globalValue = FLT_MAX;
			pol->localValue = FLT_MAX;
			pol->parent = INT_MAX;
		}

		std::list<polygon_t*> nodesToTest;
		std::list<polygon_t*> nodesDone;

		startPoly->globalValue = Distance(startPoly, endPoly);
		startPoly->localValue = 0;

		nodesToTest.push_back(startPoly);

		polygon_t* currCell;

		while (!nodesToTest.empty())
		{
			nodesToTest.sort(sorter);
			nodesToTest.reverse();

			currCell = nodesToTest.front();

			// Pop it off open list
			nodesToTest.remove(currCell);

			bool bb = false;

			for (unsigned int neighbourIndex : currCell->neighbours) {
				polygon_t* neighbour = GetById(neighbourIndex);

				if (std::find(nodesDone.begin(), nodesDone.end(), neighbour) != nodesDone.end()) {
					continue;
				}

				float newLocal = currCell->localValue + (currCell == startPoly ? Distance(from, neighbour->center) : Distance(currCell, neighbour));
				float newGlobal = newLocal + Distance(neighbour->center, to);
				if (newLocal + newGlobal < neighbour->globalValue)
				{
					neighbour->parent = currCell->id;
					neighbour->localValue = newLocal;
					neighbour->globalValue = neighbour->localValue + Distance(neighbour->center, to);
					nodesToTest.push_back(neighbour);
				}
			}

			nodesDone.push_back(currCell);

			if (bb) {
				break;
			}
		}

		path.clear();

		polygon_t* pathCell = endPoly;

		path.push_back(pathCell);

		while (pathCell != startPoly) {
			pathCell = GetById(pathCell->parent);
			if (pathCell == nullptr) {
				return {};
			}
			path.push_back(pathCell);
		}

		path.reverse();

		PullString();

		return movePath;
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

	void NavMesh::LoadFromFile(std::string mapName) {
		std::string mapFileName = "./maps/";
		mapFileName.append(mapName).append("/").append(mapName).append(".nvm");

		std::ifstream file(mapFileName);

		if (!file.is_open()) {
			std::cout << "Could not open file!" << std::endl;
			return;
		}

		mesh.clear();
		std::vector<vertex_t> vertices;

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

				vertex_t v;
				v.x = x;
				v.y = y;
				v.z = z;
				vertices.push_back(v);
			}
		}


		FindNeighbours();
	}
}