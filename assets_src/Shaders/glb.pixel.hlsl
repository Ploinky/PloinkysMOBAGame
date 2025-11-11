#include <glb.hlsli>

Texture2D shaderTexture;
SamplerState sampleType;

float4 main(GLBPixelShaderInputType input) : SV_TARGET {
	float4 textureColor;
	textureColor = shaderTexture.Sample(sampleType, input.tex);
    return textureColor;
}