#pragma once

#include "utils/pch.h"

class Texture;

class Material
{
    public:
        Material(Texture* tex);
        ~Material() = default;

        void bind(ID3D12GraphicsCommandList* cmdList, UINT rootIndex);

    private:
        Texture* texture;
};
