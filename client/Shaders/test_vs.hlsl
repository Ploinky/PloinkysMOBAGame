cbuffer perFrameBuffer
{
    float4x4 projMatrix;
    float4x4 cameraMatrix;
};

cbuffer perModelBuffer
{
    float4x4 modelMatrix;
}

cbuffer perObjectBuffer
{
    float4x4 animation_palette[256];
}

struct VertexInputType
{
    float4 position : POSITION;
    float4 normal : NORMAL;
    float2 tex : TEXCOORD0;
    float4 bone_indices : BONE_INDICES;
    float4 bone_weights : BONE_WEIGHTS;
};

struct PixelInputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
};


PixelInputType main(VertexInputType input)
{
    PixelInputType output;
    output.position = float4(0, 0, 0, 0);

    float4 in_pos = float4(input.position.xyz, 1.0f);
    
    float3 out_pos = float3(0, 0, 0);

    out_pos += input.bone_weights.x * mul(in_pos, animation_palette[input.bone_indices.x]).xyz;
    out_pos += input.bone_weights.y * mul(in_pos, animation_palette[input.bone_indices.y]).xyz;
    out_pos += input.bone_weights.z * mul(in_pos, animation_palette[input.bone_indices.z]).xyz;
    out_pos += input.bone_weights.w * mul(in_pos, animation_palette[input.bone_indices.w]).xyz;
    
    input.position = float4(out_pos, 1.0f);
    
    output.position = mul(input.position, modelMatrix);
    output.position = mul(output.position, cameraMatrix);
    output.position = mul(output.position, projMatrix);

    output.tex = input.tex;

    input.normal = float4(mul(input.normal.xyz, (float3x3) modelMatrix), 1.0f);
    output.normal = normalize(input.normal);

    return output;
}