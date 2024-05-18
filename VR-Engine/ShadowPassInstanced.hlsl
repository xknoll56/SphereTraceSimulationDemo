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