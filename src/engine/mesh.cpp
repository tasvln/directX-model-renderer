#include "mesh.h"

#include "material.h"

Mesh::Mesh(
    ComPtr<ID3D12Device2> device, 
    const std::vector<VertexStruct>& vertices,
    const std::vector<uint32_t>& indices,
    Material* mat
) : 
    material(mat)
{
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

// void Mesh::draw(
//     ID3D12GraphicsCommandList* cmdList,
//     UINT rootIndex
// ) {
//     if (material) {
//         material->bind(cmdList, rootIndex);
//     }

//     auto vbView = vertex->getView();
//     auto ibView = index->getView();
//     auto indexCount = index->getCount();

//     cmdList->IASetVertexBuffers(0, 1, &vbView);
//     cmdList->IASetIndexBuffer(&ibView);
//     cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
//     cmdList->DrawIndexedInstanced(indexCount, 1, 0, 0, 0);
// }

void Mesh::draw(
    ID3D12GraphicsCommandList* cmdList,
    UINT rootIndex
) {
    LOG_INFO(L"[Mesh] Drawing mesh with %zu vertices, %zu indices",
             vertex->getCount(),
             index->getCount());

    if (material) {
        LOG_INFO(L"[Material] Binding texture to root index %u", rootIndex);
        material->bind(cmdList, rootIndex);
    }

    auto vbView = vertex->getView();
    auto ibView = index->getView();
    auto indexCount = index->getCount();

    cmdList->IASetVertexBuffers(0, 1, &vbView);
    cmdList->IASetIndexBuffer(&ibView);
    cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    cmdList->DrawIndexedInstanced(indexCount, 1, 0, 0, 0);
}
