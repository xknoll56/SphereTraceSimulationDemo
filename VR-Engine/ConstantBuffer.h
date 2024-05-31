#pragma once
#include "stdafx.h"
#include <map>

struct ConstantBufferAccessor
{

    ID3D12Resource** m_constantBuffers;
    UINT8** m_pCbvDataBegins;
    UINT mSizeofConstantBufferData;
    D3D12_CPU_DESCRIPTOR_HANDLE* cpuDescriptorHandles;
    D3D12_GPU_DESCRIPTOR_HANDLE* gpuDescriptorHandles;
    UINT numSwaps;

    void init(ID3D12Device* pDevice, DescriptorHandleProvider& dhp, void* pConstantBufferData, UINT sizeofConstantBufferData, UINT numSwaps)
    {
        this->numSwaps = numSwaps;
        m_constantBuffers = new ID3D12Resource*[numSwaps];
        cpuDescriptorHandles = new D3D12_CPU_DESCRIPTOR_HANDLE[numSwaps];
        gpuDescriptorHandles = new D3D12_GPU_DESCRIPTOR_HANDLE[numSwaps];
        m_pCbvDataBegins = new UINT8*[numSwaps];
        // Create the constant buffer.
        for(int i = 0; i< numSwaps; i++)
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
                IID_PPV_ARGS(&m_constantBuffers[i])));

            // Describe and create a constant buffer view.
            D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc = {};
            cbvDesc.BufferLocation = m_constantBuffers[i]->GetGPUVirtualAddress();
            cbvDesc.SizeInBytes = mSizeofConstantBufferData;
            cpuDescriptorHandles[i] = dhp.getCpuHandle(pDevice);
            pDevice->CreateConstantBufferView(&cbvDesc, cpuDescriptorHandles[i]);

            // Map and initialize the constant buffer. We don't unmap this until the
            // app closes. Keeping things mapped for the lifetime of the resource is okay.
            CD3DX12_RANGE readRange(0, 0);        // We do not intend to read from this resource on the CPU.
            ThrowIfFailed(m_constantBuffers[i]->Map(0, &readRange, reinterpret_cast<void**>(&m_pCbvDataBegins[i])));
            memcpy(m_pCbvDataBegins[i], pConstantBufferData, sizeofConstantBufferData);

            gpuDescriptorHandles[i] = dhp.GpuHandleFromCpuHandle(cpuDescriptorHandles[i]);
        }
    }

    void updateConstantBufferData(void* pConstantBufferData, UINT swapIndex)
    {
        memcpy(m_pCbvDataBegins[swapIndex], pConstantBufferData, mSizeofConstantBufferData);
    }

    void bind(ID3D12GraphicsCommandList* pCommandList, UINT rootParameterIndex, UINT swapIndex)
    {
        pCommandList->SetGraphicsRootDescriptorTable(rootParameterIndex, gpuDescriptorHandles[swapIndex]);
    }
};


struct ConstantBufferAccessorStack
{
    // For each constant buffer type, we will have a stack
    std::vector<UINT> stackIndices;
    std::vector<std::vector<ConstantBufferAccessor>> cbaStacks;
    UINT numStacks;
    UINT size;



    ConstantBufferAccessorStack(UINT size)
    {
        this->size = size;
        stackIndices = std::vector<UINT>(size);
        cbaStacks = std::vector<std::vector<ConstantBufferAccessor>>(size);
        numStacks = 0;
    }

    ConstantBufferAccessorStack() : ConstantBufferAccessorStack(5)
    {

    }

    ConstantBufferAccessorStack& operator=(const ConstantBufferAccessorStack& other) 
    {
        this->cbaStacks = other.cbaStacks;
        this->cbaStacks.resize(other.size);
        this->stackIndices = other.stackIndices;
        this->stackIndices.resize(other.size);
        this->size = other.size;
        this->numStacks = other.numStacks;
        return *this;
    }

    UINT createStack(ID3D12Device* pDevice, DescriptorHandleProvider& dhp,  UINT sizeofConstantBufferData, UINT numAccessors, UINT numSwaps)
    {
        if (numStacks < size)
        {
            UINT stackIndex = numStacks;
            cbaStacks[stackIndex] = std::vector<ConstantBufferAccessor>(numAccessors);
            stackIndices[stackIndex] = 0;
            char* data = (char*)malloc(sizeofConstantBufferData);
            for (int i = 0; i < numAccessors; i++)
            {
                cbaStacks[stackIndex][i].init(pDevice, dhp, data, sizeofConstantBufferData, numSwaps);
            }
            numStacks++;
            free(data);
            return stackIndex;
        }
        return -1;
    }

    void updateCurrentAccessor(UINT stackIndex, void* pConstantBufferData, UINT swapIndex)
    {
        cbaStacks[stackIndex][stackIndices[stackIndex]].updateConstantBufferData(pConstantBufferData, swapIndex);
    }

    void bindCurrentAccessor(UINT stackIndex, ID3D12GraphicsCommandList* pCommandList, UINT rootParameterIndex, UINT swapIndex)
    {
        cbaStacks[stackIndex][stackIndices[stackIndex]].bind(pCommandList, rootParameterIndex, swapIndex);
    }

    void incrementStackIndex(UINT stackIndex)
    {
        UINT curIndex = stackIndices[stackIndex];
        if (curIndex < cbaStacks[stackIndex].size() - 1)
        {
            stackIndices[stackIndex]++;
        }
    }

    void updateBindAndIncrementCurrentAccessor(UINT stackIndex, void* pConstantBufferData, ID3D12GraphicsCommandList* pCommandList, UINT rootParameterIndex, UINT swapIndex)
    {
        cbaStacks[stackIndex][stackIndices[stackIndex]].updateConstantBufferData(pConstantBufferData, swapIndex);
        cbaStacks[stackIndex][stackIndices[stackIndex]].bind(pCommandList, rootParameterIndex, swapIndex);
        UINT curIndex = stackIndices[stackIndex];
        if (curIndex < cbaStacks[stackIndex].size() - 1)
        {
            stackIndices[stackIndex]++;
        }
    }

    void updateAndBindtCurrentAccessor(UINT stackIndex, void* pConstantBufferData, ID3D12GraphicsCommandList* pCommandList, UINT rootParameterIndex, UINT swapIndex)
    {
        cbaStacks[stackIndex][stackIndices[stackIndex]].updateConstantBufferData(pConstantBufferData, swapIndex);
        cbaStacks[stackIndex][stackIndices[stackIndex]].bind(pCommandList, rootParameterIndex, swapIndex);
    }

    void resetStackIndex(UINT stackIndex)
    {
        stackIndices[stackIndex] = 0;
    }

    void resetAllStackIndices()
    {
        for (int i = 0; i < numStacks; i++)
        {
            stackIndices[i] = 0;
        }
    }
};