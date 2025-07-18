#include <Material.h>

Material::~Material() {
	if (Texture != nullptr) {
		Texture->Release();
		Texture = nullptr;
	}
}
