#ifndef LIGHTING_STRUCTS_HLSL
#define LIGHTING_STRUCTS_HLSL

#define MAX_DIRECTIONAL_LIGHTS 4
#define MAX_POINT_LIGHTS 8
#define MAX_SPOT_LIGHTS 4

struct alignas(16) DirectionalLight {
    float3 direction;
    float  intensity;
    float3 color;
    float  pad; // padding for 16-byte alignment
};

struct alignas(16) PointLight {
    float3 position;
    float  range;
    float3 color;
    float  intensity;
};

struct alignas(16) SpotLight {
    float3 position;
    float  range;
    float3 direction;
    float  innerCone;
    float  outerCone;
    float3 color;
    float  intensity;
};

struct alignas(16) LightBufferData {
    DirectionalLight dirLights[MAX_DIRECTIONAL_LIGHTS];
    PointLight pointLights[MAX_POINT_LIGHTS];
    SpotLight spotLights[MAX_SPOT_LIGHTS];
    int numDirLights;
    int numPointLights;
    int numSpotLights;
};


#endif
