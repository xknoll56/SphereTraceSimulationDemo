cbuffer VertexShaderConstantBuffer : register(b0)
{
    float4x4 mvp;
    float4x4 model;
    float4x4 lightViewProj;
    float4 color;
    float colorMix;
};

struct PSInput
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
};

PSInput VSMain(float3 position : POSITION)
{
    PSInput result;

    result.position = mul(float4(position, 1.0f), mvp);
    result.color = color;
    return result;
}

float4 PSMain(PSInput input) : SV_TARGET
{
    return input.color;

}