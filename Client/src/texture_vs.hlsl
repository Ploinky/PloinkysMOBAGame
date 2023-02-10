cbuffer perFrameBuffer {
	float4x4 projMatrix;
	float4x4 cameraMatrix;
};

cbuffer perObjectBuffer {
	float4x4 modelMatrix;
}

struct VertexInputType {
	float4 position : POSITION;
	float2 tex : TEXCOORD0;
};

struct PixelInputType {
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
};

PixelInputType main(VertexInputType input) {
	PixelInputType output;

	input.position.w = 1.0f;

	output.position = mul(input.position, modelMatrix);
	output.position = mul(output.position, cameraMatrix);
	output.position = mul(output.position, projMatrix);

	output.tex = input.tex;

	return output;
}