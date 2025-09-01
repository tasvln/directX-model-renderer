#pragma once

#include "utils/pch.h"
#include "material.h"
#include "resources/texture.h"

class Mesh;
class DescriptorHeap;

class aiNode;
class aiScene;
class aiMesh;

class Model {
    public:
        Model(
            ComPtr<ID3D12Device2> device, 
            ComPtr<ID3D12GraphicsCommandList> cmdList, 
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

        XMFLOAT3 getBoundingCenter() const { 
            return boundingCenter; 
        }
        
        float getBoundingRadius() const { 
            return boundingRadius; 
        }

    private:
        void loadModel(const std::string& path);
        void processNode(aiNode* node, const aiScene* scene);
        std::unique_ptr<Mesh> processMesh(aiMesh* mesh, const aiScene* scene);

        // helpers
        std::wstring toWide(const std::string& s) const;
        std::wstring resolveTexturePath(const std::string& relPath) const;

        Texture* makeWhiteFallbackTexture();

    private:
        ComPtr<ID3D12Device2> device;
        ComPtr<ID3D12GraphicsCommandList> cmdList;
        DescriptorHeap* srvHeap = nullptr;

        std::string directory;
        UINT nextDescriptorIndex = 0;

        std::vector<std::unique_ptr<Mesh>> meshes;
        std::vector<std::unique_ptr<Material>> materials;
        std::vector<std::unique_ptr<Texture>> textures;

        Texture* whiteTexture;

        // bounding box -> basically to scale the model based on the camera which just makes sense...
        XMFLOAT3 globalMin;
        XMFLOAT3 globalMax;
        XMFLOAT3 boundingCenter;
        float boundingRadius = 0.0f;
};