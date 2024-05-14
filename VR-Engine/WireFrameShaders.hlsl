cbuffer ConstantBuffer : register(b0)
{
    float4x4 mvp;
};

cbuffer PixelShaderConstants : register(b1)
{
    float4 color;
};

struct PSInput
{
    float4 position : SV_POSITION;
};

PSInput VSMain(float3 position : POSITION)
{
    PSInput result;

    result.position = mul(float4(position, 1.0f), mvp);

    return result;
}

float4 PSMain(PSInput input) : SV_TARGET
{
    return color;

}