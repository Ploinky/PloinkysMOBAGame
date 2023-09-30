Texture2D shaderTexture;
SamplerState sampleType;

struct PixelInputType {
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
};

float4 main(PixelInputType input) : SV_TARGET {
    // color of the texture at the uv coordinate
	float4 textureColor;
	textureColor = shaderTexture.Sample(sampleType, input.tex);

    // color of our light
    // TODO this should not be hardcoded
    float4 diffuseColor = float4(1, 1, 1, 1);

    // direction of our diffuse light source
    // TODO this should not be hardcoded
    float3 lightDir = float3(1, 0, 0);
    
    // we need to invert the light direction for our calculation
    lightDir *= -1;
    
    // check how much light is hitting our pixel
    float lightIntensity = saturate(dot(input.normal, lightDir));

    // saturate the light's color
    float4 color = saturate(diffuseColor * lightIntensity);

    // return the final color!
    return textureColor * color;
}