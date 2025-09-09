#ifndef MATERIAL_STRUCTS_HLSL
#define MATERIAL_STRUCTS_HLSL

struct alignas(16) Material
{
    float4 emissive;
    float4 ambient;
    float4 diffuse;
    float4 specular;
    float  specularPower;
    bool   useTexture;
    float2 padding;
};

#endif