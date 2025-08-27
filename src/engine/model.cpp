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
    LOG_INFO(L"Model -> Loading from path: %hs", path.c_str());
    loadModel(path);
    LOG_INFO(L"Model -> Finished loading model: %hs", path.c_str());
}

void Model::loadModel(const std::string& path) {
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
        throw std::runtime_error("Assimp failed to load model: " + std::string(importer.GetErrorString()));
    }

    LOG_INFO(L"Model -> Scene loaded. Mesh count: %u, Root node children: %u",
        scene->mNumMeshes, scene->mRootNode->mNumChildren);

    processNode(scene->mRootNode, scene);
}

void Model::processNode(aiNode* node, const aiScene* scene) {
    LOG_DEBUG(L"Model -> Processing node: %hs, Meshes: %u, Children: %u",
        node->mName.C_Str(),
        node->mNumMeshes,
        node->mNumChildren);

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

        // Texcoord (only first set for now)
        if (mesh->mTextureCoords[0]) {
            vertex.texcoord = {
                mesh->mTextureCoords[0][i].x,
                mesh->mTextureCoords[0][i].y
            };
        }

        vertices.push_back(vertex);
    }

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
    LOG_DEBUG(L"Model -> Drawing %zu meshes", meshes.size());
    for (auto& mesh : meshes) {
        mesh->draw(cmdList);
    }
}
