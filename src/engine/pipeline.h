#pragma once

#include "utils/pch.h"
#include "shader.h"

class Pipeline {
    public:
        Pipeline(
            ComPtr<ID3D12Device2> device,
            const Shader& vertexShader,
            const Shader& pixelShader,
            const std::vector<D3D12_INPUT_ELEMENT_DESC>& inputLayout,
            const std::vector<D3D12_ROOT_PARAMETER>& rootParams = {},
            const std::vector<D3D12_STATIC_SAMPLER_DESC>& samplers = {},
            DXGI_FORMAT rtvFormat = DXGI_FORMAT_R8G8B8A8_UNORM,
            DXGI_FORMAT dsvFormat = DXGI_FORMAT_D24_UNORM_S8_UINT
        );

        ~Pipeline() = default;

        ComPtr<ID3D12PipelineState> getPipelineState() const { 
            return pipelineState; 
        }

        ComPtr<ID3D12RootSignature> getRootSignature() const { 
            return rootSignature; 
        }

        inline D3D12_STATIC_SAMPLER_DESC getDefaultStaticSampler() {
            D3D12_STATIC_SAMPLER_DESC sampler = {};
            sampler.Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
            sampler.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
            sampler.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
            sampler.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
            sampler.MipLODBias = 0.0f;
            sampler.MaxAnisotropy = 1;
            sampler.ComparisonFunc = D3D12_COMPARISON_FUNC_ALWAYS;
            sampler.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
            sampler.MinLOD = 0.0f;
            sampler.MaxLOD = D3D12_FLOAT32_MAX;
            sampler.ShaderRegister = 0; // s0
            sampler.RegisterSpace = 0;
            sampler.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
            return sampler;
        }

    private:
        ComPtr<ID3D12PipelineState> pipelineState;
        ComPtr<ID3D12RootSignature> rootSignature;
};