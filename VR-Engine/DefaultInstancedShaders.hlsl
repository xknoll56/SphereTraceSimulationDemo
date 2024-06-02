
#define MAX_INSTANCES 400
cbuffer VertexShaderConstants : register(b0)
{
    float4x4 viewProjection;
    float4x4 lightViewProjs[2];
    float4x4 model[MAX_INSTANCES];
    float4 colors[MAX_INSTANCES];
    
};

struct SpotLight
{
    float3 position;
    float range;
    float3 direction;
    float spot;
    float3 color;
    float intentsity;
};

cbuffer PixelShaderConstants : register(b1)
{
    float4 cameraPos;
    float4 lightDir;
    SpotLight spotLights[15];
    int numSpotLights;
    int numShadowTextures;
}

struct PSInput
{
    float4 position : SV_POSITION;
    float3 normal : NORMAL; // Surface normal
    float4 color : COLOR;
    float4 lightSpacePos[2] : TEXCOORD;
    float4 modelPos : TEXCOORD2;
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
    float4x4 mvp = mul(model[instanceID], viewProjection);
    result.position = mul(float4(position, 1.0f), mvp);
    result.normal = normalize(mul(normal, extractRot(model[instanceID])));
    result.color = colors[instanceID];
    result.modelPos = mul(float4(position, 1.0f), model[instanceID]);
    result.lightSpacePos[0] = mul(result.modelPos, lightViewProjs[0]);
    result.lightSpacePos[1] = mul(result.modelPos, lightViewProjs[1]);
    return result;
}

Texture2D<float> shadowTexture : register(t0);
Texture2D<float> shadowTexture1 : register(t1);
SamplerComparisonState shadowSampler : register(s0);

// Phong lighting model function
float computeSpotlight(SpotLight light, PSInput input, float shadowFactor)
{
    float dist = length(light.position - input.modelPos.xyz);
    float attenuation = 1.0 / (1.0f + (0.02f * dist) + (0.01f * dist * dist));
    float3 viewDir = normalize(light.direction - input.position.xyz);
    float3 normalizedLightDir = normalize(-light.direction);
    float3 halfVec = normalize(normalizedLightDir + viewDir);
    float diffuse = max(0.0f, dot(input.normal, normalizedLightDir));
    diffuse *= attenuation;
    float specular = pow(max(0.0f, dot(input.normal, halfVec)), 32) * attenuation;
    float theta = acos(dot(light.direction, (1.0f / dist) * (input.modelPos.xyz - light.position)));
    float ambient = 0.01f;
    if(theta<3.14159f*0.333f)
        return (diffuse * shadowFactor + ambient + specular * shadowFactor);
    else
        return ambient;
}

float computeDirectionalLight(PSInput input, float shadowFactor)
{
    float3 viewDir = normalize(cameraPos.xyz - input.position.xyz);
    float3 normalizedLightDir = normalize(lightDir.xyz);
    float3 halfVec = normalize(normalizedLightDir + viewDir);
    float diffuse = max(0.0f, dot(input.normal, normalizedLightDir));
    float specular = pow(max(0.0f, dot(input.normal, halfVec)), 32);
    float ambient = 0.3f;
    return (diffuse * shadowFactor + ambient + specular * shadowFactor);
}

float calculateShadowFactor(float4 lightSpacePos, Texture2D<float> shadowText)
{
    
    uint width;
    uint height;
    shadowText.GetDimensions(width, height);
    float2 texelSize = { 1.0f / width, 1.0f / height };
    
    float2 shadowTexCoords;
    shadowTexCoords.x = 0.5f + (lightSpacePos.x / lightSpacePos.w * 0.5f);
    shadowTexCoords.y = 0.5f - (lightSpacePos.y / lightSpacePos.w * 0.5);

    float pixelDepth = lightSpacePos.z / lightSpacePos.w;
    float epsilon = 0.0001f;

    float shadowFactor = 0.0f;
    if ((saturate(shadowTexCoords.x) == shadowTexCoords.x) &&
			(saturate(shadowTexCoords.y) == shadowTexCoords.y) &&
			(pixelDepth < 1.0f))
    {
        float sum = 0.0f;
        for (int i = -1; i <= 1; i++)
        {
            for (int j = -1; j <= 1; j++)
            {
                float2 offsetTexCoords = shadowTexCoords + float2(i, j) * texelSize;
                sum += shadowText.SampleCmpLevelZero(shadowSampler, offsetTexCoords, pixelDepth);
            }
        }
        shadowFactor = sum / 9.0f; 
    }
    else
    {
        shadowFactor = 1.0f;
    }
    return shadowFactor;
}


float4 PSMain(PSInput input) : SV_TARGET
{

    float3 baseColor = input.color.xyz;
    float alpha = input.color.w;
    

    float shadowFactor = 1.0f;
    if(numShadowTextures > 0)
        shadowFactor = calculateShadowFactor(input.lightSpacePos[0], shadowTexture);
    
    float overallLightValue = 1.0f;
    if (numSpotLights > 0)
        overallLightValue = computeSpotlight(spotLights[0], input, shadowFactor);
    
    shadowFactor = 1.0f;
    if(numShadowTextures>1)
        shadowFactor *= calculateShadowFactor(input.lightSpacePos[1], shadowTexture1);

    if(numSpotLights>1)
        overallLightValue += computeSpotlight(spotLights[1], input, shadowFactor);
    
    
    for (int i = 2; i < numSpotLights; i++)
        overallLightValue += computeSpotlight(spotLights[i], input, shadowFactor);
    
    
    overallLightValue = saturate(overallLightValue);
    return float4(overallLightValue * baseColor, alpha);

}