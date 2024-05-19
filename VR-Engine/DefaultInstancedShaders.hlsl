
#define MAX_INSTANCES 400
cbuffer VertexShaderConstants : register(b0)
{
    float4x4 mvp[MAX_INSTANCES];
    float4x4 model[MAX_INSTANCES];
    float4 colors[MAX_INSTANCES];
    float4x4 lightViewProj;
    
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
    float4 color : COLOR;
    float4 lightSpacePos : TEXCOORD;
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

PSInput VSMain(float3 position : POSITION, float3 normal : NORMAL, float2 uv : TEXCOORD, uint instanceID : SV_InstanceID)
{
    PSInput result;

    result.position = mul(float4(position, 1.0f), mvp[instanceID]);
    result.normal = normalize(mul(normal, extractRot(model[instanceID])));
    result.color = colors[instanceID];
    float4 modelPos = mul(float4(position, 1.0f), model[instanceID]);
    result.lightSpacePos = mul(modelPos, lightViewProj);
    return result;
}

Texture2D<float> shadowTexture : register(t0);
SamplerComparisonState shadowSampler : register(s0);


float4 PSMain(PSInput input) : SV_TARGET
{
    float3 viewDir = normalize(cameraPos.xyz - input.position.xyz);
    float3 normalizedLightDir = normalize(lightDir.xyz);
    float3 halfVec = normalize(normalizedLightDir + viewDir);
    float diffuse = max(0.0f, dot(input.normal, normalizedLightDir));
    float specular = pow(max(0.0f, dot(input.normal, halfVec)), 32); // Adjust the specular power as needed
    float4 baseColor = input.color;
    float ambient = 0.3f;
    //return baseColor * (diffuse + ambient + specular);
    
    float margin = asin(saturate(diffuse));
    float epsilon = 0.001 / margin;
    //float epsilon = 0.05;
    epsilon = clamp(epsilon, 0.0, 0.1);
    
    uint width;
    uint height;
    shadowTexture.GetDimensions(width, height);
    
    float2 shadowTexCoords;
    shadowTexCoords.x = 0.5f + (input.lightSpacePos.x / input.lightSpacePos.w * 0.5f);
    shadowTexCoords.y = 0.5f - (input.lightSpacePos.y / input.lightSpacePos.w * 0.5);

    float pixelDepth = input.lightSpacePos.z / input.lightSpacePos.w;
    

    float shadowFactor = 0.0f;
    if ((saturate(shadowTexCoords.x) == shadowTexCoords.x) &&
			(saturate(shadowTexCoords.y) == shadowTexCoords.y) &&
			(pixelDepth < 1.0f))
    {
        shadowFactor = shadowTexture.SampleCmpLevelZero(shadowSampler, shadowTexCoords, pixelDepth - epsilon);
    }
    else
    {
        shadowFactor = 1.0f;
    }
    
    float4 lighting = (diffuse * shadowFactor + ambient + specular * shadowFactor) * baseColor;
    return lighting;

}