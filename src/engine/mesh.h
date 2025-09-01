#pragma once

#include "utils/pch.h"
#include "engine/resources/vertex.h"
#include "engine/resources/index.h"

class Material;

class Mesh {
    public:
        Mesh(
            ComPtr<ID3D12Device2> device, 
            const std::vector<VertexStruct>& vertices,
            const std::vector<uint32_t>& indices,
            Material* mat = nullptr
        );

        ~Mesh() = default;

        VertexBuffer* getVertex() const {
            return vertex.get();
        }   

        IndexBuffer* getIndex() const {
            return index.get();
        } 

        void draw(
            ID3D12GraphicsCommandList* cmdList,
            UINT rootIndex
        );

    private:
        std::unique_ptr<VertexBuffer> vertex;
        std::unique_ptr<IndexBuffer> index;

        Material* material = nullptr;
};