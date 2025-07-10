#pragma once

#include <d3d11.h>

class Material {
public:
	~Material();

	ID3D11ShaderResourceView* Texture;
};
