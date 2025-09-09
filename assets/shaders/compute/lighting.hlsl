#include "structs/lighting.hlsl"

// L -> Light vector
// N -> Surface Normal
// H -> Half-angle vector?
// V -> View vector

cbuffer MaterialCB : register(b0)
{
    Material material;
};

cbuffer LightCB : register(b1)
{
    LightBufferData lightData;
    // Camera position
    float3 eyePosition;  
    // 1 = Blinn-Phong, 0 = Phong
    float useBlinnPhong;
    // Ambient factor
    float3 globalAmbient;
};

float3 diffuse(float3 N, float3 L, float3 lightColor)
{
    return lightColor * saturate(dot(N, L));
}

float3 specular(float3 N, float3 L, float3 V, float3 lightColor, float specPower)
{
    if (useBlinnPhong > 0.5)
    {
        float3 H = normalize(L + V);
        return lightColor * pow(saturate(dot(N, H)), specPower);
    }
    else
    {
        float3 R = reflect(-L, N);
        return lightColor * pow(saturate(dot(R, V)), specPower);
    }
}

float attenuation(float distance, float range)
{
    return saturate(1.0f - distance / range);
}

float3 computeDirectionalLight(
    float3 normal, 
    float3 diffuse,
    float3 worldPosition,
    DirectionalLight light
) {
    
    return result;
}

float3 computePointLight(
    float3 normal, 
    float3 diffuse,
    float3 worldPosition,
    PointLight light
) {
    return;
}

float3 computeSpotLight(
    float3 normal, 
    float3 diffuse,
    float3 worldPosition,
    SpotLight light
) {
    return;
}

float3 computeLighting(
    float3 normal, 
    float3 diffuse,
    float3 worldPosition,
    LightBufferData lightData
) {
    return;
}