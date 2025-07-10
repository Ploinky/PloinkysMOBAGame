struct GUIPixelShaderInputType {
    float4 position : SV_POSITION;
    float2 textureCoordinate : TEXCOORD0;
};

struct GUIVertexShaderInputType {
    float2 pos : POSITION;
    float2 texCoord : TEXCOORD0;
};

GUIPixelShaderInputType main(GUIVertexShaderInputType inVal) {
    GUIPixelShaderInputType outVal;
    
    outVal.position = float4(inVal.pos.x, inVal.pos.y, 0, 1);
    outVal.textureCoordinate = inVal.texCoord;

    return outVal;
}