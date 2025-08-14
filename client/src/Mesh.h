#pragma once

#include <core/graphics/graphics-engine.h>

class Mesh {
public:
	~Mesh();

	BufferHandle_t VertexBuffer;
	BufferHandle_t IndexBuffer;
	int IndexCount;
	int MaterialIndex = -1;
};
