void main(uint vid : SV_VertexID, out float4 positionOut : SV_POSITION)
{
    if (vid == 0) {
        positionOut = float4(-1, 1, 0, 1.0f);
    }
    if (vid == 1) {
        positionOut = float4(-1, -3, 0, 1.0f);
    }
    if (vid == 2) {
        positionOut = float4(3, 1, 0, 1.0f);
    }
}