#pragma once

#include <DirectXMath.h>

#include <cstdint>

using namespace DirectX;

constexpr UINT MAX_LIGHTS = 16;

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
    XMMATRIX model;
    XMMATRIX viewProj;
};

// Lighting Struct
enum class LightType : int {
    Directional = 0,
    Point       = 1,
    Spot        = 2
};

struct alignas(16) Light
{
    XMFLOAT4 position;
    XMFLOAT4 direction;
    XMFLOAT4 color;
    float range = 0.0f;
    float innerAngle = 0.0f;
    float outerAngle = 0.0f;
    float intensity = 1.0f;
    int type = 0;
    float enabled = 0.0f; // match HLSL float instead of bool
    float pad[2] = {0,0}; // align to 16 bytes
};

struct alignas(16) LightBufferData
{
    XMFLOAT4 eyePosition;
    XMFLOAT4 globalAmbient;
    Light lights[MAX_LIGHTS];
    UINT numLights;
    float useBlinnPhong = 0.0f; // match HLSL
    float specPower = 16.0f;
    float pad[2] = {0,0}; // align to 16 bytes
};

struct alignas(16) MaterialData
{
    XMFLOAT4 emissive;       // 16 bytes
    XMFLOAT4 ambient;        // 16 bytes
    XMFLOAT4 diffuse;        // 16 bytes
    XMFLOAT4 specular;       // 16 bytes
    float specularPower;  // 4 bytes
    bool useTexture;     // 4 bytes
    XMFLOAT2 padding;    // 8 bytes to match 16-byte alignment
};
