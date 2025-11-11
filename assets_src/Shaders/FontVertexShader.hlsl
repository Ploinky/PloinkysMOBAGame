void main(
    in float2 inPosition : POSITION,
    in float2 inTextureCoord : TEXCOORD0,
    in uint inLetter : CHAR,
    in float inOffset : POSITION1,
    out float4 position : SV_POSITION,
    out float2 textureCoord : TEXCOORD,
    out uint letter : CHAR
)
{
    textureCoord = inTextureCoord;
    letter = min(max(inLetter, 0), 256);
    position = float4(inPosition.x + inOffset, inPosition.y, 0, 1);
}