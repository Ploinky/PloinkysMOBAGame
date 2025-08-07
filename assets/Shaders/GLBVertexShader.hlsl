#include <GLBShader.hlsli>

cbuffer perFrameBuffer {
	float4x4 projMatrix;
	float4x4 cameraMatrix;
};

cbuffer perObjectBuffer {
	float4x4 modelMatrix;
}

cbuffer perMeshBuffer {
    matrix boneTransforms[256];
};

struct VertexInputType {
	float3 position : POSITION;
    float3 normal : NORMAL;
	float2 tex : TEXCOORD0;
	uint4 joints: JOINTS0;
	float4 weights: WEIGHTS0;
};

GLBPixelShaderInputType main(VertexInputType input) {
	/*
	GLBPixelShaderInputType output;

	float4 pos = float4(input.position, 1.0f);

	pos = mul(pos, modelMatrix);
	pos = mul(pos, cameraMatrix);
	pos = mul(pos, projMatrix);

	output.position = pos;
	output.tex = input.tex;

	return output;
	*/
    GLBPixelShaderInputType output;

    // Skinning
	float4 inputPosition = float4(input.position.xyz,1);
    float3 skinnedPosition = float3(0, 0, 0);
    float3 skinnedNormal = float3(0.0, 0.0, 0.0);
    
	if(input.weights.x + input.weights.y + input.weights.z + input.weights.w > 0) {
    	skinnedPosition += input.weights.x * mul(inputPosition, boneTransforms[input.joints.x]).xyz;
		skinnedPosition += input.weights.y * mul(inputPosition, boneTransforms[input.joints.y]).xyz;
		skinnedPosition += input.weights.z * mul(inputPosition, boneTransforms[input.joints.z]).xyz;
		skinnedPosition += input.weights.w * mul(inputPosition, boneTransforms[input.joints.w]).xyz;
	} else {
		skinnedPosition = input.position.xyz;
	}

	inputPosition = float4(skinnedPosition, 1);

	output.position = mul(inputPosition, modelMatrix);
	output.position = mul(output.position, cameraMatrix);
	output.position = mul(output.position, projMatrix);

    output.tex = input.tex;


    return output;
}