// I do realise i can't be perfect with shaders in 1 day/week and it'd take a bit for experience
//  to master or somewhat fully understand what i'm doing but glad i understand the math aspect of
//  things... that being said, i need more experience and need to read up more

#include "compute/lighting.hlsl"

struct PixelInputType {
    float4 position : SV_POSITION;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float2 uv : TEXCOORD;
};

cbuffer LightCB : register(b1)
{
    LightBufferData lightData;
};

// Texture SRV bound to t0
Texture2D texture : register(t0);

// Sampler bound to s0
SamplerState samplerLinear : register(s0);

float4 psmain(PixelInputType input) : SV_TARGET
{
    // Sample texture
    float4 color = texture.Sample(samplerLinear, input.uv);

    float3 light = computeLighting(

    );

    return float4(light, color.a);
}
