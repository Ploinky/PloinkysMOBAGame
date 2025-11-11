Texture2D shaderTexture;
SamplerState sampleType;


struct FontPixelShaderInputType {
};

float4 main(
    in float4 position : SV_POSITION,
    in float2 textureCoord : TEXCOORD0,
    in uint inLetter : CHAR
) : SV_TARGET {
    float letter = inLetter;
    float letterU = (letter % 16.0f) / 16.0f;
    float letterV = (int) (letter / 16.0f) / 16.0f;
    
    float2 texCoord = float2(letterU, letterV);
    texCoord.x += textureCoord.x / 16.0f;
    texCoord.y += textureCoord.y / 16.0f;
    return shaderTexture.Sample(sampleType, texCoord);
}