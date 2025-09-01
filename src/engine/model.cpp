#pragma once

#include "model.h"
#include "mesh.h"
#include "descriptor_heap.h"

#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

namespace fs = std::filesystem;

Model::Model(
    ComPtr<ID3D12Device2> device, 
    ComPtr<ID3D12GraphicsCommandList> cmdList, 
    DescriptorHeap* srvHeap, 
    const std::string& path
) :
    device(device), 
    cmdList(cmdList), 
    srvHeap(srvHeap)
{
    // store model folder for resolving relative texture paths
    try {
        directory = fs::path(path).parent_path().string();
    } catch (...) {
        directory.clear();
    }

    loadModel(path);
}

void Model::loadModel(const std::string& path) {
    LOG_INFO(L"Model -> Loading from path: %hs", path.c_str());

    Assimp::Importer importer;

    const aiScene* scene = importer.ReadFile(
        path,
        aiProcess_Triangulate |
        aiProcess_CalcTangentSpace |
        aiProcess_GenSmoothNormals |
        aiProcess_FlipUVs
    );

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        LOG_ERROR(L"Model -> Assimp failed: %hs", importer.GetErrorString());
        throw std::runtime_error("Assimp failed to load model");
    }

    // Reset bounds
    globalMin = { FLT_MAX,  FLT_MAX,  FLT_MAX };
    globalMax = { -FLT_MAX, -FLT_MAX, -FLT_MAX };

    processNode(scene->mRootNode, scene);

    // Compute global bounding sphere
    XMFLOAT3 extent = {
        (globalMax.x - globalMin.x) * 0.5f,
        (globalMax.y - globalMin.y) * 0.5f,
        (globalMax.z - globalMin.z) * 0.5f
    };

    boundingCenter = {
        (globalMin.x + globalMax.x) * 0.5f,
        (globalMin.y + globalMax.y) * 0.5f,
        (globalMin.z + globalMax.z) * 0.5f
    };

    boundingRadius = std::sqrt(
        extent.x * extent.x +
        extent.y * extent.y +
        extent.z * extent.z
    );

    LOG_INFO(L"Model -> Global bounds Min(%.2f, %.2f, %.2f), "
             L"Max(%.2f, %.2f, %.2f), Radius=%.2f",
        globalMin.x, globalMin.y, globalMin.z,
        globalMax.x, globalMax.y, globalMax.z,
        boundingRadius);
}

void Model::processNode(aiNode* node, const aiScene* scene) {
    // Process all the meshes at this node
    for (unsigned int i = 0; i < node->mNumMeshes; i++) {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        meshes.push_back(processMesh(mesh, scene));
    }

    // Then process all children
    for (unsigned int i = 0; i < node->mNumChildren; i++) {
        processNode(node->mChildren[i], scene);
    }
}

std::unique_ptr<Mesh> Model::processMesh(aiMesh* mesh, const aiScene* scene) {
    LOG_INFO(L"Model -> Processing mesh: %hs, Vertices: %u, Faces: %u",
        mesh->mName.C_Str(),
        mesh->mNumVertices,
        mesh->mNumFaces
    );

    std::vector<VertexStruct> vertices;
    std::vector<uint32_t> indices;

    vertices.reserve(mesh->mNumVertices);
    indices.reserve(mesh->mNumFaces * 3); // all faces are triangles

    XMFLOAT3 minPos = { FLT_MAX,  FLT_MAX,  FLT_MAX };
    XMFLOAT3 maxPos = { -FLT_MAX, -FLT_MAX, -FLT_MAX };

    // Process vertices
    for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
        VertexStruct vertex{};

        // Position
        vertex.position = {
            mesh->mVertices[i].x,
            mesh->mVertices[i].y,
            mesh->mVertices[i].z,
            1.0f
        };

        // Normal
        if (mesh->HasNormals()) {
            vertex.normal = {
                mesh->mNormals[i].x,
                mesh->mNormals[i].y,
                mesh->mNormals[i].z
            };
        }

        // Tangent
        if (mesh->HasTangentsAndBitangents()) {
            vertex.tangent = {
                mesh->mTangents[i].x,
                mesh->mTangents[i].y,
                mesh->mTangents[i].z
            };
        }

        // Texcoord (first set)
        if (mesh->mTextureCoords[0]) {
            vertex.texcoord = {
                mesh->mTextureCoords[0][i].x,
                mesh->mTextureCoords[0][i].y
            };
        } else {
            vertex.texcoord = { 0.0f, 0.0f };
        }

        vertices.push_back(vertex);

        // Update local bounds
        minPos.x = std::min(minPos.x, mesh->mVertices[i].x);
        minPos.y = std::min(minPos.y, mesh->mVertices[i].y);
        minPos.z = std::min(minPos.z, mesh->mVertices[i].z);

        maxPos.x = std::max(maxPos.x, mesh->mVertices[i].x);
        maxPos.y = std::max(maxPos.y, mesh->mVertices[i].y);
        maxPos.z = std::max(maxPos.z, mesh->mVertices[i].z);
    }

    // Indices
    for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }

    // Update global bounds
    globalMin.x = std::min(globalMin.x, minPos.x);
    globalMin.y = std::min(globalMin.y, minPos.y);
    globalMin.z = std::min(globalMin.z, minPos.z);

    globalMax.x = std::max(globalMax.x, maxPos.x);
    globalMax.y = std::max(globalMax.y, maxPos.y);
    globalMax.z = std::max(globalMax.z, maxPos.z);

    LOG_DEBUG(
        L"Model -> Mesh processed. Final vertex count: %zu, index count: %zu",
        vertices.size(), indices.size()
    );

    // Create material if the mesh has a diffuse or base color texture
    Texture* texForMesh = nullptr;

    if (mesh->mMaterialIndex >= 0) {
        aiMaterial* aimat = scene->mMaterials[mesh->mMaterialIndex];
        aiString texPath;

        if ((aimat->GetTextureCount(aiTextureType_BASE_COLOR) > 0 && 
             aimat->GetTexture(aiTextureType_BASE_COLOR, 0, &texPath) == AI_SUCCESS) ||
            (aimat->GetTextureCount(aiTextureType_DIFFUSE) > 0 && 
             aimat->GetTexture(aiTextureType_DIFFUSE, 0, &texPath) == AI_SUCCESS)) {
            
            std::string texStr(texPath.C_Str());
            if (!texStr.empty()) {
                std::wstring wpath = resolveTexturePath(texStr);
                textures.push_back(std::make_unique<Texture>(
                    device, cmdList, srvHeap, wpath, nextDescriptorIndex++
                ));
                texForMesh = textures.back().get();
            }
        }
    }

    if (!texForMesh) {
        if (!whiteTexture) whiteTexture = makeWhiteFallbackTexture();
        texForMesh = whiteTexture;
    }

    materials.push_back(std::make_unique<Material>(texForMesh));
    Material* matPtr = materials.back().get();

    return std::make_unique<Mesh>(device, vertices, indices, matPtr);
}

// void Model::draw(
//     ID3D12GraphicsCommandList* cmdList,
//     ID3D12DescriptorHeap* srvHeap,
//     UINT rootIndex
// ) {
//     // Set descriptor heap ONCE
//     ID3D12DescriptorHeap* heaps[] = { srvHeap };
//     cmdList->SetDescriptorHeaps(1, heaps);

//     for (auto& mesh : meshes) {
//         mesh->draw(cmdList, rootIndex); 
//     }
// }

void Model::draw(
    ID3D12GraphicsCommandList* cmdList,
    ID3D12DescriptorHeap* srvHeap,
    UINT rootIndex
) {
    LOG_INFO(L"[Model] Drawing %zu meshes", meshes.size());

    // Set descriptor heap once
    ID3D12DescriptorHeap* heaps[] = { srvHeap };
    cmdList->SetDescriptorHeaps(1, heaps);

    for (size_t i = 0; i < meshes.size(); ++i) {
        LOG_INFO(L"[Model] Drawing mesh %zu/%zu", i + 1, meshes.size());
        meshes[i]->draw(cmdList, rootIndex);
    }
}

std::wstring Model::resolveTexturePath(const std::string& texRel) const {
    fs::path p(texRel);
    if (p.is_absolute()) 
        return p.wstring();  // use .wstring() to get a wstring

    return (fs::path(directory) / p).wstring(); // combine and convert
}

Texture* Model::makeWhiteFallbackTexture() {
    std::wstring whitePath = DEFAULT_WHITE_TEXTURE; // path to your 1x1 white texture
    textures.push_back(std::make_unique<Texture>(device, cmdList, srvHeap, whitePath, nextDescriptorIndex++));
    return textures.back().get();
}
