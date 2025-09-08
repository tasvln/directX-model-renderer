#ifndef LIGHTING_STRUCTS_HLSL
#define LIGHTING_STRUCTS_HLSL

struct DirectionalLight {
    float3 direction;
    float  intensity;
    float3 color;
    float  pad1; // Padding for 16-byte alignment
};

struct PointLight {
    float3 position;
    float  range;
    float3 color;
    float  intensity;
};

struct LightBufferData {
    DirectionalLight directionalLight;
    PointLight pointLight;
};

#endif
