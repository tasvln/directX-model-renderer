#pragma once

#include "material.h"
#include "engine/resources/texture.h"

Material::Material(Texture* tex) : texture(tex) {}

void Material::bind(ID3D12GraphicsCommandList* cmdList, UINT rootIndex) {
    if (!texture) {
        LOG_INFO(L"[Material] No texture to bind");
        return;
    }

    LOG_INFO(L"[Material] Binding texture to root index %u", rootIndex);
    cmdList->SetGraphicsRootDescriptorTable(rootIndex, texture->getGPUHandle());
    LOG_INFO(L"[Material] Binding texture completed", rootIndex);
}
