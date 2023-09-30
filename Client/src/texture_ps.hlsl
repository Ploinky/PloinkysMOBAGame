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
    
    // color of our ambient light
    // TODO this should not be hardcoded
    float4 ambientColor = float4(0.7, 0.7, 0.7, 1);
    
    // all pixels start out as ambient lighted
    float4 color = ambientColor;

    // direction of our diffuse light source
    // TODO this should not be hardcoded
    float3 lightDir = float3(1, 0, 0);
    
    // we need to invert the light direction for our calculation
    lightDir *= -1;
    
    // check how much light is hitting our pixel
    float lightIntensity = saturate(dot(input.normal, lightDir));
    
    
    // add diffuse light to ambient light if there is any
    if (lightIntensity > 0.0f)
    {
        color += (diffuseColor * lightIntensity);
    }

    // saturate the light's color
    color = saturate(color);

    // return the final color!
    return textureColor * color;
}