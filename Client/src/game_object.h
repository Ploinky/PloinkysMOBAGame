#pragma once

#include "mesh.h"

namespace PMG {
	class GameObject {
	public:
		~GameObject();

		unsigned long net_id;
		unsigned int health;
		unsigned int max_health;

		Mesh* mesh;
	};
}