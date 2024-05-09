#pragma once
#include "stdafx.h"


struct DescriptorHandleProvider
{
    ID3D12DescriptorHeap* m_cbvHeap;
    UINT numDescriptors;
    UINT numDescriptorsProvided;

    void init(ID3D12Device* pDevice, UINT numDescriptors)
    {
        this->numDescriptors = numDescriptors;
        this->numDescriptorsProvided = 0;

        D3D12_DESCRIPTOR_HEAP_DESC cbvHeapDesc = {};
        cbvHeapDesc.NumDescriptors = numDescriptors;
        cbvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
        cbvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
        ThrowIfFailed(pDevice->CreateDescriptorHeap(&cbvHeapDesc, IID_PPV_ARGS(&m_cbvHeap)));
    }

    D3D12_CPU_DESCRIPTOR_HANDLE getCpuHandle(ID3D12Device* pDevice)
    {
        if (numDescriptorsProvided < numDescriptors)
        {
            D3D12_CPU_DESCRIPTOR_HANDLE handle = m_cbvHeap->GetCPUDescriptorHandleForHeapStart();
            handle.ptr += pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV) * numDescriptorsProvided;
            numDescriptorsProvided++;
            return handle;
        }
    }

    D3D12_GPU_DESCRIPTOR_HANDLE GpuHandleFromCpuHandle(D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle)
    {
        int offset = (int)(cpuHandle.ptr - m_cbvHeap->GetCPUDescriptorHandleForHeapStart().ptr);
        return CD3DX12_GPU_DESCRIPTOR_HANDLE(m_cbvHeap->GetGPUDescriptorHandleForHeapStart(), offset);
    }

    void bindDescriptorHeap(ID3D12GraphicsCommandList* m_commandList)
    {
        ID3D12DescriptorHeap* ppHeaps[] = { m_cbvHeap };
        m_commandList->SetDescriptorHeaps(1, ppHeaps);
    }
};