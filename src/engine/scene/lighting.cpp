#include "lighting.h"

Lighting::Lighting(
    ComPtr<ID3D12Device2> device
) {
    lightCBV = std::make_unique<ConstantBuffer>(
        device,
        static_cast<UINT>(sizeof(LightBufferData))
    );

    lightData.numDirectionalLights = 0;
    lightData.numPointLights = 0;
    lightData.numSpotLights = 0;
}

void Lighting::setDirectionalLight(
    const XMFLOAT3& direction, 
    const XMFLOAT3& color, 
    float intensity
) {
    if (lightData.numDirectionalLights >= MAX_DIRECTIONAL_LIGHTS) 
        return;

    auto& light = lightData.directionalLights[lightData.numDirectionalLights++];
    light.type = LightType::Directional;
    light.direction = direction;
    light.color = color;
    light.intensity = intensity;
}

void Lighting::setPointLight(
    const XMFLOAT3& position, 
    float range, 
    const XMFLOAT3& color, 
    float intensity
) {
    if (lightData.numPointLights >= MAX_POINT_LIGHTS) 
        return;

    auto& light = lightData.pointLights[lightData.numPointLights++];
    light.type = LightType::Point;
    light.position = position;
    light.range = range;
    light.color = color;
    light.intensity = intensity;
}

void Lighting::setSpotLight(
    const XMFLOAT3& position, 
    const XMFLOAT3& direction, 
    float range, 
    float innerAngle, 
    float outerAngle, 
    const XMFLOAT3& color, 
    float intensity
) {
    if (lightData.numSpotLights >= MAX_SPOT_LIGHTS) 
        return;

    auto& light = lightData.spotLights[lightData.numSpotLights++];
    light.type = LightType::Spot;
    light.position = position;
    light.direction = direction;
    light.range = range;
    light.innerAngle = innerAngle;
    light.outerAngle = outerAngle;
    light.color = color;
    light.intensity = intensity;
}

void Lighting::updateGPU() {
    lightCBV->update(&lightData, sizeof(LightBufferData));
}

