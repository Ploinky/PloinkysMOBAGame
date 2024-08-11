Texture2D shaderTexture;
SamplerState sampleType;

struct GUIPixelShaderInputType {
    float4 position : SV_POSITION;
    float2 textureCoordinate : TEXCOORD;
};

float4 main(GUIPixelShaderInputType inval) : SV_TARGET {
    return shaderTexture.Sample(sampleType, inval.textureCoordinate);
}