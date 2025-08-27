#pragma once

#include "utils/pch.h"

class Mesh;

class aiNode;
class aiScene;
class aiMesh;

class Model {
    public:
        Model(ComPtr<ID3D12Device2> device, const std::string& path);
        ~Model() = default;

        void draw(ID3D12GraphicsCommandList* cmdList);
    private:
        ComPtr<ID3D12Device2> device;
        std::vector<std::unique_ptr<Mesh>> meshes;

        void loadModel(const std::string& path);
        void processNode(aiNode* node, const aiScene* scene);
        std::unique_ptr<Mesh> processMesh(aiMesh* mesh, const aiScene* scene);
};