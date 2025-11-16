#include <glb.hlsli>

cbuffer perFrameBuffer {
	row_major float4x4 projMatrix;
	row_major float4x4 cameraMatrix;
};

cbuffer perObjectBuffer {
	row_major float4x4 modelMatrix;
}

cbuffer perMeshBuffer {
    row_major float4x4 boneTransforms[256];
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
    	skinnedPosition += input.weights.x * mul(boneTransforms[input.joints.x], inputPosition).xyz;
		skinnedPosition += input.weights.y * mul(boneTransforms[input.joints.y], inputPosition).xyz;
		skinnedPosition += input.weights.z * mul(boneTransforms[input.joints.z], inputPosition).xyz;
		skinnedPosition += input.weights.w * mul(boneTransforms[input.joints.w], inputPosition).xyz;
	} else {
		skinnedPosition = input.position.xyz;
	}

	inputPosition = float4(skinnedPosition, 1);

	output.position = mul(modelMatrix, inputPosition);
	output.position = mul(cameraMatrix, output.position);
	output.position = mul(projMatrix, output.position);

    output.tex = input.tex;


    return output;
}