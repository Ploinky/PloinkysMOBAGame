#include <Mesh.h>

Mesh::~Mesh() {
	if (VertexBuffer != nullptr) {
		VertexBuffer->Release();
		VertexBuffer = nullptr;
	}

	if (IndexBuffer != nullptr) {
		IndexBuffer->Release();
		IndexBuffer = nullptr;
	}
}
