#pragma once

#include "utils/pch.h"
#include "engine/resources/constant.h"

class Lighting {
public:
    Lighting(
        ComPtr<ID3D12Device2> device
    );

    void setDirectionalLight(
        const XMFLOAT3& direction, 
        const XMFLOAT3& color, 
        float intensity
    );

    void setPointLight(
        const XMFLOAT3& position, 
        float range, 
        const XMFLOAT3& color, 
        float intensity
    );

    void setSpotLight(
        const XMFLOAT3& position, 
        const XMFLOAT3& direction, 
        float range, float innerAngle, 
        float outerAngle, 
        const XMFLOAT3& color, 
        float intensity
    );

    // Update GPU buffer
    void updateGPU();

    // Get GPU handle
    ConstantBuffer* getCBV() { return lightCBV.get(); }

private:
    LightBufferData lightData {};
    std::unique_ptr<ConstantBuffer> lightCBV;
};