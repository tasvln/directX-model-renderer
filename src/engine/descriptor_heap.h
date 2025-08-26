#pragma once

#include "utils/pch.h"

class DescriptorHeap {
    public:
        DescriptorHeap(
            ComPtr<ID3D12Device2> device, 
            D3D12_DESCRIPTOR_HEAP_TYPE type, 
            UINT numDescriptors, 
            bool shaderVisible = false
        );

        CD3DX12_CPU_DESCRIPTOR_HANDLE getCPUHandle(UINT index) const;
        CD3DX12_GPU_DESCRIPTOR_HANDLE getGPUHandle(UINT index) const;

        ComPtr<ID3D12DescriptorHeap> getHeap() const { 
            return heap; 
        }

        UINT getDescriptorSize() const { 
            return descriptorSize; 
        }

    private:
        ComPtr<ID3D12DescriptorHeap> heap;
        UINT descriptorSize;
        D3D12_DESCRIPTOR_HEAP_TYPE type;
};