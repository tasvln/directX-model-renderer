#pragma once

#include "buffer/vertex.h"
#include "buffer/index.h"

class Mesh {
    public:
        Mesh(
            ComPtr<ID3D12Device2> device, 
            const std::vector<VertexStruct>& vertices,
            const std::vector<uint32_t>& indices
        );
        ~Mesh() = default;

        VertexBuffer* getVertex() const {
            return vertex.get();
        }   
        IndexBuffer* getIndex() const {
            return index.get();
        }   

    private:
        std::unique_ptr<VertexBuffer> vertex;
        std::unique_ptr<IndexBuffer> index;
};