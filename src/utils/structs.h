#pragma once

#include <DirectXMath.h>

#include <cstdint>

using namespace DirectX;

constexpr UINT MAX_DIRECTIONAL_LIGHTS = 4;
constexpr UINT MAX_POINT_LIGHTS = 8;
constexpr UINT MAX_SPOT_LIGHTS = 4;

// future -> can put this in a settings file if i decide on a debug UI
struct WindowConfig {
    LPCWSTR appName;
    LPCWSTR windowClassName;
    uint32_t width;
    uint32_t height;
    bool enabledDirectX;
    bool useWarp;
    bool fullscreen = false;
    bool resizable = true;
};

struct alignas(16) VertexStruct {
    XMFLOAT4 position;
    XMFLOAT3 normal;
    XMFLOAT3 tangent;
    XMFLOAT2 texcoord;
};

struct alignas(256) MVPConstantStruct
{
    XMMATRIX mvp;
};

// Lighting Struct
enum class LightType { 
    Directional, 
    Point, 
    Spot 
};

struct alignas(16) LightBase {
    LightType type;
    float intensity;
    XMFLOAT3 color;
    float pad0; // padding
};

struct alignas(16) DirectionalLight : LightBase {
    XMFLOAT3 direction;
    float pad1;
};

struct alignas(16) PointLight : LightBase {
    XMFLOAT3 position;
    float range;
};

struct alignas(16) SpotLight : LightBase {
    XMFLOAT3 position;
    float range;
    XMFLOAT3 direction;
    float innerAngle;
    float outerAngle;
    float pad2;
};

struct alignas(16) LightBufferData {
    DirectionalLight directionalLights[MAX_DIRECTIONAL_LIGHTS];
    PointLight pointLights[MAX_POINT_LIGHTS];
    SpotLight spotLights[MAX_SPOT_LIGHTS];
    UINT numDirectionalLights;
    UINT numPointLights;
    UINT numSpotLights;
    float pad3[1];
};