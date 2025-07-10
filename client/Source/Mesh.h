#pragma once

#include <d3d11.h>

class Mesh {
public:
	~Mesh();

	ID3D11Buffer* VertexBuffer;
	ID3D11Buffer* IndexBuffer;
	int IndexCount;
	int MaterialIndex = -1;
};
