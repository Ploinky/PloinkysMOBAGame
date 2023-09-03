#pragma once

#include <vector>
#include <string>
#include <list>

namespace PMG {
	typedef struct {
		float x;
		float y;
		float z;
	} vertex_t;

	bool operator==(const vertex_t& lhs, const vertex_t& rhs);

	vertex_t operator-(const vertex_t& lhs, const vertex_t& rhs);

	vertex_t CenterOf(vertex_t v1, vertex_t v2, vertex_t v3);

	extern unsigned int polygonId;

	typedef struct {
		unsigned int id;
		vertex_t center;
		vertex_t vertices[3];
		std::vector<unsigned int> neighbours;
		float globalValue;
		float localValue;
		unsigned int parent;
	} polygon_t;

	bool sorter(const polygon_t* first, const polygon_t* second);

	bool operator==(const polygon_t& lhs, const polygon_t& rhs);

	bool operator!=(const polygon_t& lhs, const polygon_t& rhs);

	class NavMesh {
	public:
		std::vector<polygon_t*> mesh;
		std::list<polygon_t*> path;
		std::list<vertex_t> movePath;
		vertex_t from;
		vertex_t to;

		NavMesh();
		void LoadFromFile(std::string mapName);
		std::list<vertex_t> PlanPath(vertex_t from, vertex_t to);
		bool PointInMesh(vertex_t pt);

	private:
		void FindNeighbours();
		void PullString();
		polygon_t* FindPolygonAt(vertex_t pt);
		polygon_t* GetById(unsigned int id);
		polygon_t* CreatePolygon(vertex_t v1, vertex_t v2, vertex_t v3);
		float Sign(vertex_t p1, vertex_t p2, vertex_t p3);
		bool PointInTriangle(vertex_t pt, vertex_t v1, vertex_t v2, vertex_t v3);
		bool IsNeighbour(polygon_t poly, polygon_t potentialNeighbour);
		float Distance(vertex_t a, vertex_t b);
		float Distance(polygon_t* a, polygon_t* b);
		float Cross(const vertex_t v1, const vertex_t v2);
		float AngleBetween(vertex_t a, vertex_t b);
	};

	typedef struct {
		std::list<vertex_t> path;
		vertex_t target;
	} nav_agent_t;

	typedef struct {
		polygon_t* from;
		polygon_t* to;
		vertex_t left;
		vertex_t right;
	} portal_t;
}