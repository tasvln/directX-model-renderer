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

void Mesh::draw(ID3D12GraphicsCommandList* cmdList) {
    auto vbView = vertex->getView();
    auto ibView = index->getView();
    auto indexCount = index->getCount();
    
    cmdList->IASetVertexBuffers(0, 1, &vbView);
    cmdList->IASetIndexBuffer(&ibView);
    cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    cmdList->DrawIndexedInstanced(indexCount, 1, 0, 0, 0);
}