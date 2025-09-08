// I do realise i can't be perfect with shaders in 1 day/week and it'd take a bit for experience
//  to master or somewhat fully understand what i'm doing but glad i understand the math aspect of
//  things... that being said, i need more experience and need to read up more

#include "structs/lighting.hlsl"

struct PixelInputType {
    float4 position : SV_POSITION;  // required
    float3 normal   : NORMAL;
    float3 tangent  : TANGENT;
    float2 uv       : TEXCOORD;
};

cbuffer LightCB : register(b1)
{
    LightBufferData lightData;
};

// Texture SRV bound to t0
Texture2D tex : register(t0);

// Sampler bound to s0
SamplerState samLinear : register(s0);

float4 psmain(PixelInputType input) : SV_TARGET
{
    // Sample texture
    float4 color = tex.Sample(samLinear, input.uv);

    // TODO: read into lighting -> that's what's next!!!
    // Simple directional lighting
    float3 lightDir = normalize(float3(0.5f, 1.0f, -0.5f));
    float NdotL = saturate(dot(normalize(input.normal), lightDir));

    float3 litColor = color.rgb * (0.2f + 0.8f * NdotL);

    return float4(litColor, color.a);
}
