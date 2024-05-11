//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// This code is licensed under the MIT License (MIT).
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************

cbuffer SceneConstantBuffer : register(b0)
{
    float4x4 mvp;
};

struct PSInput
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD;
};

Texture2D g_texture : register(t0);
SamplerState g_sampler : register(s0);

PSInput VSMain(float3 position : POSITION, float3 normal : NORMAL, float2 uv : TEXCOORD)
{
    PSInput result;

    result.position = mul(float4(position, 1.0f), mvp);
    result.uv = uv;

    return result;
}

float4 PSMain(PSInput input) : SV_TARGET
{
    //return float4(input.uv.x, input.uv.y, 0, 1);
    return g_texture.Sample(g_sampler, input.uv);
}

//struct PSInput
//{
//    float4 position : SV_POSITION;
//    float2 color : TEXCOORD;
//};

//Texture2D g_texture : register(t0);
//SamplerState g_sampler : register(s0);

//PSInput VSMain(float4 position : POSITION, float2 color : TEXCOORD)
//{
//    PSInput result;

//    result.position = position + offset;
//    result.color = color;

//    return result;
//}

//float4 PSMain(PSInput input) : SV_TARGET
//{
//    return g_texture.Sample(g_sampler, input.color);

//}
