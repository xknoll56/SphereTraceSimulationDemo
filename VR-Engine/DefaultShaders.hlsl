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

cbuffer VertexShaderConstantBuffer : register(b0)
{
    float4x4 mvp;
    float4x4 model;
    float4x4 lightViewProj; 
    float4 color;
    float colorMix;
};

cbuffer PixelShaderConstants : register(b1)
{
    float4 cameraPos;
    float4 lightDir;
    float4 lightColor; 

}

struct PSInput
{
    float4 position : SV_POSITION;
    float3 normal : NORMAL; // Surface normal
    float2 uv : TEXCOORD;
    float4 color : COLOR;
    float colorMix : TEXCOORD1;
    float4 lightSpacePos : TEXCOORD2;
};

Texture2D g_texture : register(t0);
SamplerState g_sampler : register(s0);

Texture2D<float> shadowTexture : register(t1);
SamplerComparisonState shadowSampler : register(s1)
{
    Filter = D3D12_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;
    AddressU = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
    AddressV = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
    AddressW = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
    ComparisonFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL; // Specify the comparison function
    BorderColor = float4(0, 0, 0, 0); // Adjust as needed
    MaxAnisotropy = 1; // Adjust as needed
    MaxLOD = D3D12_FLOAT32_MAX;
    MinLOD = 0;
};

float3x3 extractRot(float4x4 model)
{
    float3 s;
    s.x = sqrt(model._m00 * model._m00 + model._m10 * model._m10 + model._m20 * model._m20);
    s.y = sqrt(model._m01 * model._m01 + model._m11 * model._m11 + model._m21 * model._m21);
    s.z = sqrt(model._m02 * model._m02 + model._m12 * model._m12 + model._m22 * model._m22);
    float3x3 ret;
    ret._m00_m01_m02 = float3(model._m00 / s.x, model._m01 / s.y, model._m02 / s.z);
    ret._m10_m11_m12 = float3(model._m10 / s.x, model._m11 / s.y, model._m12 / s.z);
    ret._m20_m21_m22 = float3(model._m20 / s.x, model._m21 / s.y, model._m22 / s.z);
    return ret;
}

float4 CalculateLighting(float3 viewDir, float3 normal, float3 lightDir, float3 lightColor)
{
    float3 halfVec = normalize(lightDir + viewDir);
    float diffuse = max(0.0f, dot(normal, lightDir));
    float specular = pow(max(0.0f, dot(normal, halfVec)), 32); // Adjust the specular power as needed
    float ambient = 0.5f;
    return float4(1, 1, 1, 1);
}

PSInput VSMain(float3 position : POSITION, float3 normal : NORMAL, float2 uv : TEXCOORD)
{
    PSInput result;

    result.position = mul(float4(position, 1.0f), mvp);
    result.normal = normalize(mul(normal, extractRot(model)));
    result.uv = uv;
    result.color = color;
    result.colorMix = colorMix;

    result.lightSpacePos = mul(float4(position, 1.0f), lightViewProj);
    
    return result;
}

float4 PSMain(PSInput input) : SV_TARGET
{
    //float3 viewDir = normalize(cameraPos.xyz - input.position.xyz);
    //float3 normalizedLightDir = normalize(lightDir.xyz);
    //float3 halfVec = normalize(normalizedLightDir + viewDir);
    //float diffuse = max(0.0f, dot(input.normal, normalizedLightDir));
    //float specular = pow(max(0.0f, dot(input.normal, halfVec)), 32); // Adjust the specular power as needed
    //float4 baseColor = lerp(g_texture.Sample(g_sampler, input.uv), input.color, input.colorMix);
    //float ambient = 0.3f;

    
   // float margin = asin(saturate(diffuse));
    //float epsilon = 0.001 / margin;
    float epsilon = 0.05;
   // epsilon = clamp(epsilon, 0.0, 0.1);
    
    uint width;
    uint height;
    shadowTexture.GetDimensions(width, height);

    float pixelDepth = input.lightSpacePos.z / input.lightSpacePos.w;
    float shadowFactor = shadowTexture.SampleCmpLevelZero(shadowSampler, input.lightSpacePos.xy / input.lightSpacePos.w, pixelDepth);

    float lighting = 0.0f;
    if(shadowFactor>epsilon)
        lighting = 1.0f;
    // Apply shadow to the lighting
    //float4 lighting = (diffuse * shadowFactor + ambient + specular * shadowFactor) * lightColor;
    //float4 lighting = (diffuse + ambient + specular) * lightColor;

    return lighting * float4(1, 1, 1, 1);

}