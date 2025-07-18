#pragma once

#include <DirectXMath.h>
#include <Direct3d.h>
#include <Common/AssetManager.h>
#include <exception>

typedef struct {
	DirectX::XMFLOAT4X4 projMatrix;
	DirectX::XMFLOAT4X4 cameraMatrix;
} FlatUnlitShaderFrameConst_t;

typedef struct {
	DirectX::XMFLOAT4X4 modelMatrix;
} FlatUnlitShaderObjectConst_t;

typedef struct {
	float position[3];
	float color[4];
} FlatUnlitShaderVertex_t;

class CFlatUnlitShader {
public:
	CFlatUnlitShader();
	~CFlatUnlitShader();

	void Initialize(Direct3D* direct3D, AssetManager* assetManager);
	
	ID3D11Buffer* m_pFrameConstBuffer;
	FlatUnlitShaderFrameConst_t m_frameConstData;
	ID3D11Buffer* m_pModelConstBuffer;
	FlatUnlitShaderObjectConst_t m_modelConstData;
	ID3D11VertexShader* m_pVertexShader;
	ID3D11PixelShader* m_pPixelShader;
	ID3D11InputLayout* m_pInputLayout;
};
