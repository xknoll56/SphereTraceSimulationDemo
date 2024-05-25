#define MAX_INSTANCES 400
cbuffer VertexShaderConstants : register(b0)
{
    float4x4 viewProjection;
    float4x4 lightViewProj;
    float4x4 model[MAX_INSTANCES];
    float4 colors[MAX_INSTANCES];
    
};
struct PSInput
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
};

PSInput VSMain(float3 position : POSITION, uint instanceID : SV_InstanceID)
{
    PSInput result;

    float4x4 mvp = mul(model[instanceID], viewProjection);
    result.position = mul(float4(position, 1.0f), mvp);
    result.color = colors[instanceID];
    return result;
}

float4 PSMain(PSInput input) : SV_TARGET
{
    return input.color;

}