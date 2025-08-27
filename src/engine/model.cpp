#include "model.h"
#include "mesh.h"

#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

Model::Model(
    ComPtr<ID3D12Device2> device,
    const std::string& path
) :
    device(device)
{
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
        mesh->mNumFaces);

    std::vector<VertexStruct> vertices;
    std::vector<uint32_t> indices;

    vertices.reserve(mesh->mNumVertices);

    XMFLOAT3 minPos = { FLT_MAX,  FLT_MAX,  FLT_MAX };
    XMFLOAT3 maxPos = { -FLT_MAX, -FLT_MAX, -FLT_MAX };

    for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
        VertexStruct vertex{};

        // Position
        vertex.position = {
            mesh->mVertices[i].x,
            mesh->mVertices[i].y,
            mesh->mVertices[i].z,
            1.0f
        };

        // Update local bounds
        minPos.x = std::min(minPos.x, mesh->mVertices[i].x);
        minPos.y = std::min(minPos.y, mesh->mVertices[i].y);
        minPos.z = std::min(minPos.z, mesh->mVertices[i].z);

        maxPos.x = std::max(maxPos.x, mesh->mVertices[i].x);
        maxPos.y = std::max(maxPos.y, mesh->mVertices[i].y);
        maxPos.z = std::max(maxPos.z, mesh->mVertices[i].z);

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

        // Texcoord (only first set for now)
        if (mesh->mTextureCoords[0]) {
            vertex.texcoord = {
                mesh->mTextureCoords[0][i].x,
                mesh->mTextureCoords[0][i].y
            };
        }

        vertices.push_back(vertex);
    }

    // Update global bounds
    globalMin.x = std::min(globalMin.x, minPos.x);
    globalMin.y = std::min(globalMin.y, minPos.y);
    globalMin.z = std::min(globalMin.z, minPos.z);

    globalMax.x = std::max(globalMax.x, maxPos.x);
    globalMax.y = std::max(globalMax.y, maxPos.y);
    globalMax.z = std::max(globalMax.z, maxPos.z);

    // Indices
    for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }

    LOG_DEBUG(L"Model -> Mesh processed. Final vertex count: %zu, index count: %zu",
        vertices.size(), indices.size());

    return std::make_unique<Mesh>(device, vertices, indices);
}

void Model::draw(ID3D12GraphicsCommandList* cmdList) {
    for (auto& mesh : meshes) {
        mesh->draw(cmdList);
    }
}
