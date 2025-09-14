// I do realise i can't be perfect with shaders in 1 day/week and it'd take a bit for experience
//  to master or somewhat fully understand what i'm doing but glad i understand the math aspect of
//  things... that being said, i need more experience and need to read up more

#include "lighting.hlsl"

struct Material
{
    float4 emissive;
    float4 ambient;
    float4 diffuse;
    float4 specular;
    float specularPower;
    float useTexture;
    float2 padding; // align to 16 bytes
};

// Material
cbuffer MaterialCB : register(b1)
{
    Material material;
};

// Lights
cbuffer LightCB : register(b2)
{
    Light lights[MAX_LIGHTS];
    uint numLights;
    float4 eyePosition;
    float4 globalAmbient;
    float useBlinnPhong;
    float specPower;
    float padding[2]; // align to 16 bytes
};

// Texture/Sampler
Texture2D TextureMap : register(t0);
SamplerState SamplerWrap : register(s0);

struct PixelInputType {
    float4 position : SV_POSITION;
    float3 worldPos   : WORLDPOS;
    float3 worldNormal: NORMAL0;
    float3 tangent  : TANGENT;
    float2 uv       : TEXCOORD;
};

float4 psmain(PixelInputType IN) : SV_TARGET
{
    float3 ambient, diffuse, specular;

    computeLighting(
        lights,
        numLights,
        eyePosition.xyz,
        globalAmbient.xyz,
        (useBlinnPhong > 0.5f),
        specPower,
        IN.worldPos,
        normalize(IN.worldNormal),
        ambient,
        diffuse,
        specular
    );

    float4 texColor = material.useTexture ? TextureMap.Sample(SamplerWrap, IN.uv) : float4(1,1,1,1);

    float3 lit = material.emissive.rgb + material.ambient.rgb + ambient + diffuse + specular;

    float4 finalColor = float4(saturate(lit), 1.0f) * texColor;

    return finalColor; 
}
