
#define MAX_INSTANCES 400
cbuffer VertexShaderConstantBuffer : register(b0)
{
    float4x4 mvp[MAX_INSTANCES];
};

struct PSInput
{
    float4 position : SV_POSITION;
};


PSInput VSMain(float3 position : POSITION, uint instanceID : SV_InstanceID)
{
    PSInput result;
    result.position = mul(float4(position, 1.0f), mvp[instanceID]);
    return result;
}

float4 PSMain(PSInput input) : SV_TARGET
{
    return float4(1, 1, 1, 1);
}