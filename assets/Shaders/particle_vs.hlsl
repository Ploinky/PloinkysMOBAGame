cbuffer perFrameBuffer
{
    float4x4 projMatrix;
    float4x4 cameraMatrix;
    float4x4 billboard_matrix;
}

cbuffer perObjectBuffer
{
    float4x4 modelMatrix;
}

void main(float3 position : POSITION, float2 tex_coord : TEXCOORD0, float3 instance_position : POSITION1, out float4 positionOut : SV_POSITION, out float2 tex : TEXCOORD0, out float3 normal : NORMAL)
{
    positionOut = float4(position, 1.0f);

    // all particles face towards the camera position relative to the particle systems origin
    positionOut = mul(positionOut, billboard_matrix);
    
    // transformation of this particle instance inside the particle system
    positionOut.x += instance_position.x;
    positionOut.y += instance_position.y;
    positionOut.z += instance_position.z;
    
    // transform particle system to view space
    positionOut = mul(positionOut, modelMatrix);
    positionOut = mul(positionOut, cameraMatrix);
    positionOut = mul(positionOut, projMatrix);
    
    normal = float3(1, 0, 0);
    
    tex = tex_coord;
}