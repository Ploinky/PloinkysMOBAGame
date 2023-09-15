cbuffer perFrameBuffer
{
    float4x4 projMatrix;
    float4x4 cameraMatrix;
}

cbuffer perObjectBuffer
{
    float4x4 modelMatrix;
}

void main(float3 position : POSITION, float4 color : COLOR, float3 instance_position : POSITION1, out float4 positionOut : SV_POSITION, out float4 colorOut : COLOR)
{
    positionOut = float4(position, 1.0f);
    
    positionOut.x += instance_position.x;
    positionOut.y += instance_position.y;
    positionOut.z += instance_position.z;
    
    positionOut = mul(positionOut, modelMatrix);
    positionOut = mul(positionOut, cameraMatrix);
    positionOut = mul(positionOut, projMatrix);

    colorOut = color;
}