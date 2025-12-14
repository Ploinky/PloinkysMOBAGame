#pragma once

#include <core/graphics/graphics-engine.h>

class Mesh {
public:
	~Mesh();

	HBuffer VertexBuffer;
	HBuffer IndexBuffer;
	int IndexCount;
	int MaterialIndex = -1;
};
