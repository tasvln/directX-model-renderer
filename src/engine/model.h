#pragma once

#include "utils/pch.h"
#include "material.h"
#include "resources/texture.h"

class Mesh;
class DescriptorHeap;
class CommandQueue;

class aiNode;
class aiScene;
class aiMesh;

class Model {
    public:
        Model(
            ComPtr<ID3D12Device2> device, 
            CommandQueue* uploadQueue, 
            DescriptorHeap* srvHeap, 
            const std::string& path
        );

        ~Model() = default;

        // Draw the model. rootIndex is the root parameter index in the root signature
        // that expects the SRV descriptor table (e.g. slot 1 in your pipeline).
        void draw(
            ID3D12GraphicsCommandList* cmdList, 
            ID3D12DescriptorHeap* srvHeap,
            UINT rootIndex
        );

        XMFLOAT3 getBoundingCenter() const { return boundingCenter; }
        float getBoundingRadius() const { return boundingRadius; }

    private:
        void loadModel(const std::string& path);
        void processNode(aiNode* node, const aiScene* scene);
        std::unique_ptr<Mesh> processMesh(aiMesh* mesh, const aiScene* scene);

        // Helpers
        std::wstring toWide(const std::string& s) const;
        std::wstring resolveTexturePath(const std::string& relPath) const;

        // Creates a 1x1 white fallback texture
        std::shared_ptr<Texture> makeWhiteFallbackTexture();

    private:
        ComPtr<ID3D12Device2> device;

        CommandQueue* uploadQueue = nullptr;
        ComPtr<ID3D12GraphicsCommandList2> uploadCmdList;

        DescriptorHeap* srvHeap = nullptr;

        std::string directory;
        UINT nextDescriptorIndex = 0;

        std::vector<std::unique_ptr<Mesh>> meshes;
        std::vector<std::shared_ptr<Material>> materials; 
        std::vector<std::shared_ptr<Texture>> textures;   

        std::shared_ptr<Texture> whiteTexture; 

        // Bounding box
        XMFLOAT3 globalMin;
        XMFLOAT3 globalMax;
        XMFLOAT3 boundingCenter;
        float boundingRadius = 0.0f;
};
