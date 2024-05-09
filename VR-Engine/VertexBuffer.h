#pragma once

#include "stdafx.h"

struct Vertex
{
    ST_Vector3 position;
    ST_Vector3 normal;
    ST_Vector2 uv;
};


struct VertexBuffer
{
    ID3D12Resource* m_vertexBuffer;
    D3D12_VERTEX_BUFFER_VIEW m_vertexBufferView;
    UINT numVerts;

    void init(ID3D12Device* pDevice, float* triangleVertices, UINT sizeofVerts, UINT stride)
    {
        // Note: using upload heaps to transfer static data like vert buffers is not 
        // recommended. Every time the GPU needs it, the upload heap will be marshalled 
        // over. Please read up on Default Heap usage. An upload heap is used here for 
        // code simplicity and because there are very few verts to actually transfer.
        CD3DX12_HEAP_PROPERTIES heapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
        CD3DX12_RESOURCE_DESC resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(sizeofVerts);
        ThrowIfFailed(pDevice->CreateCommittedResource(
            &heapProperties,
            D3D12_HEAP_FLAG_NONE,
            &resourceDesc,
            D3D12_RESOURCE_STATE_GENERIC_READ,
            nullptr,
            IID_PPV_ARGS(&m_vertexBuffer)));

        // Copy the triangle data to the vertex buffer.
        UINT8* pVertexDataBegin;
        CD3DX12_RANGE readRange(0, 0);        // We do not intend to read from this resource on the CPU.
        ThrowIfFailed(m_vertexBuffer->Map(0, &readRange, reinterpret_cast<void**>(&pVertexDataBegin)));
        memcpy(pVertexDataBegin, triangleVertices, sizeofVerts);
        m_vertexBuffer->Unmap(0, nullptr);

        // Initialize the vertex buffer view.
        m_vertexBufferView.BufferLocation = m_vertexBuffer->GetGPUVirtualAddress();
        m_vertexBufferView.StrideInBytes = stride;
        m_vertexBufferView.SizeInBytes = sizeofVerts;
        this->numVerts = sizeofVerts / stride;
    }


    void draw(ID3D12GraphicsCommandList* pCommandList)
    {
        pCommandList->IASetVertexBuffers(0, 1, &m_vertexBufferView);
        pCommandList->DrawInstanced(numVerts, 1, 0, 0);
    }
};