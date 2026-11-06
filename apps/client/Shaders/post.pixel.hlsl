Texture2D shaderTexture;

cbuffer postBuffer
{
    float saturation;
	float3 padding;
}

float4 main(float4 position : SV_POSITION) : SV_TARGET {
	float desat = 1 - saturation;
	float4 texCol = shaderTexture.Load(int3(position.xy, 0));
	float avg = 0.2126 * texCol.x + 0.7152 * texCol.y + 0.0722 * texCol.z;
	return float4(
		avg * desat + texCol.x * saturation,
		avg * desat + texCol.y * saturation,
		avg * desat + texCol.z * saturation,
		texCol.w
	);
}