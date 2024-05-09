#pragma once
#include "stdafx.h"

struct ConstantBufferAccessor
{

    ID3D12Resource* m_constantBuffer;
    UINT8* m_pCbvDataBegin;
    UINT mSizeofConstantBufferData;
    D3D12_CPU_DESCRIPTOR_HANDLE cpuDescriptorHandle;
    D3D12_GPU_DESCRIPTOR_HANDLE gpuDescriptorHandle;

    void init(ID3D12Device* pDevice, DescriptorHandleProvider& dhp, void* pConstantBufferData, UINT sizeofConstantBufferData)
    {

        // Create the constant buffer.
        {
            mSizeofConstantBufferData = sizeofConstantBufferData;

            CD3DX12_HEAP_PROPERTIES heapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
            CD3DX12_RESOURCE_DESC resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(mSizeofConstantBufferData);
            ThrowIfFailed(pDevice->CreateCommittedResource(
                &heapProperties,
                D3D12_HEAP_FLAG_NONE,
                &resourceDesc,
                D3D12_RESOURCE_STATE_GENERIC_READ,
                nullptr,
                IID_PPV_ARGS(&m_constantBuffer)));

            // Describe and create a constant buffer view.
            D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
            cbvDesc.BufferLocation = m_constantBuffer->GetGPUVirtualAddress();
            cbvDesc.SizeInBytes = mSizeofConstantBufferData;
            cpuDescriptorHandle = dhp.getCpuHandle(pDevice);
            pDevice->CreateConstantBufferView(&cbvDesc, cpuDescriptorHandle);

            // Map and initialize the constant buffer. We don't unmap this until the
            // app closes. Keeping things mapped for the lifetime of the resource is okay.
            CD3DX12_RANGE readRange(0, 0);        // We do not intend to read from this resource on the CPU.
            ThrowIfFailed(m_constantBuffer->Map(0, &readRange, reinterpret_cast<void**>(&m_pCbvDataBegin)));
            memcpy(m_pCbvDataBegin, pConstantBufferData, sizeofConstantBufferData);

            gpuDescriptorHandle = dhp.GpuHandleFromCpuHandle(cpuDescriptorHandle);
        }
    }

    void updateConstantBufferData(void* pConstantBufferData)
    {
        memcpy(m_pCbvDataBegin, pConstantBufferData, mSizeofConstantBufferData);
    }

    void bind(ID3D12GraphicsCommandList* pCommandList)
    {
        pCommandList->SetGraphicsRootDescriptorTable(0, gpuDescriptorHandle);
    }
};