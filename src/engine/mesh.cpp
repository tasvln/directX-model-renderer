#include "mesh.h"

Mesh::Mesh(
    ComPtr<ID3D12Device2> device, 
    const std::vector<VertexStruct>& vertices,
    const std::vector<uint32_t>& indices
) {
    LOG_INFO(L"MeshBuffer -> Creating vertex and index buffers...");
    vertex = std::make_unique<VertexBuffer>(
        device,
        vertices
    );
    
    index = std::make_unique<IndexBuffer>(
        device,
        indices
    );

    LOG_INFO(L"MeshBuffer -> Buffers created successfully.");
}