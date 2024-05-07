//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// This code is licensed under the MIT License (MIT).
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************

#pragma once

#include "DXSample.h"

using namespace DirectX;

// Note that while ComPtr is used to manage the lifetime of resources on the CPU,
// it has no understanding of the lifetime of resources on the GPU. Apps must account
// for the GPU lifetime of resources to avoid destroying objects that may still be
// referenced by the GPU.
// An example of this can be found in the class method: OnDestroy().
using Microsoft::WRL::ComPtr;


struct RootSigniture
{
    ID3D12RootSignature* pRootSigniture;
    D3D12_FEATURE_DATA_ROOT_SIGNATURE featureData;

    RootSigniture()
    {
        // This is the highest version the sample supports. If CheckFeatureSupport succeeds, the HighestVersion returned will not be greater than this.
        featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_1;
    }

    ~RootSigniture()
    {
        pRootSigniture->Release();
    }


    void init(ID3D12Device* pDevice, CD3DX12_ROOT_PARAMETER1* rootParameters, UINT numRootParameters, D3D12_ROOT_SIGNATURE_FLAGS flags, D3D12_STATIC_SAMPLER_DESC samplerDesc)
    {

        if (FAILED(pDevice->CheckFeatureSupport(D3D12_FEATURE_ROOT_SIGNATURE, &featureData, sizeof(featureData))))
        {
            featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_0;
        }

        CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc;
        rootSignatureDesc.Init_1_1(numRootParameters, rootParameters, 1, &samplerDesc, flags);

        ComPtr<ID3DBlob> signature;
        ComPtr<ID3DBlob> error;
        ThrowIfFailed(D3DX12SerializeVersionedRootSignature(&rootSignatureDesc, featureData.HighestVersion, &signature, &error));
        ThrowIfFailed(pDevice->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&pRootSigniture)));
    }
};

struct Pipeline
{
    ID3D12PipelineState* pPipelineState;

    void init(ID3D12Device* pDevice, RootSigniture& rootSigniture, LPCWSTR vertexShaderPath, LPCWSTR pixelShaderPath,
        D3D12_INPUT_ELEMENT_DESC* inputElementDescs, UINT inputElementSize)
    {
        ComPtr<ID3DBlob> vertexShader;
        ComPtr<ID3DBlob> pixelShader;

#if defined(_DEBUG)
        // Enable better shader debugging with the graphics debugging tools.
        UINT compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
        UINT compileFlags = 0;
#endif

        ThrowIfFailed(D3DCompileFromFile((LPCWSTR)vertexShaderPath, nullptr, nullptr, "VSMain", "vs_5_0", compileFlags, 0, &vertexShader, nullptr));
        ThrowIfFailed(D3DCompileFromFile((LPCWSTR)pixelShaderPath, nullptr, nullptr, "PSMain", "ps_5_0", compileFlags, 0, &pixelShader, nullptr));

        // Describe and create the graphics pipeline state object (PSO).
        D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
        psoDesc.InputLayout = { inputElementDescs, inputElementSize };
        psoDesc.pRootSignature = rootSigniture.pRootSigniture;
        psoDesc.VS = CD3DX12_SHADER_BYTECODE(vertexShader.Get());
        psoDesc.PS = CD3DX12_SHADER_BYTECODE(pixelShader.Get());
        psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
        psoDesc.RasterizerState.FrontCounterClockwise = TRUE;
        psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
        CD3DX12_DEPTH_STENCIL_DESC1 depthDesc(D3D12_DEFAULT);
        psoDesc.DepthStencilState = depthDesc;
        psoDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
        psoDesc.SampleMask = UINT_MAX;
        psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
        psoDesc.NumRenderTargets = 1;
        psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
        psoDesc.SampleDesc.Count = 1;

        ThrowIfFailed(pDevice->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&pPipelineState)));
    }

};

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

struct ConstantBufferAccessor
{
    
    ID3D12Resource* m_constantBuffer;
    UINT8* m_pCbvDataBegin;
    UINT mSizeofConstantBufferData;
    UINT accessorIndex;
    static UINT numAccessors;

    void init(ID3D12Device* pDevice, ID3D12DescriptorHeap* m_cbvHeap, void* pConstantBufferData, UINT sizeofConstantBufferData)
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
            D3D12_CPU_DESCRIPTOR_HANDLE handle = m_cbvHeap->GetCPUDescriptorHandleForHeapStart();
            handle.ptr += pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)*numAccessors;
            accessorIndex = numAccessors;
            numAccessors++;
            pDevice->CreateConstantBufferView(&cbvDesc, handle);

            // Map and initialize the constant buffer. We don't unmap this until the
            // app closes. Keeping things mapped for the lifetime of the resource is okay.
            CD3DX12_RANGE readRange(0, 0);        // We do not intend to read from this resource on the CPU.
            ThrowIfFailed(m_constantBuffer->Map(0, &readRange, reinterpret_cast<void**>(&m_pCbvDataBegin)));
            memcpy(m_pCbvDataBegin, pConstantBufferData, sizeofConstantBufferData);
        }
    }

    void updateConstantBufferData(void* pConstantBufferData)
    {
        memcpy(m_pCbvDataBegin, pConstantBufferData, mSizeofConstantBufferData);
    }

    void bind(ID3D12GraphicsCommandList* pCommandList, ID3D12Device* pDevice, ID3D12DescriptorHeap* m_cbvHeap)
    {
        pCommandList->SetGraphicsRootDescriptorTable(0, CD3DX12_GPU_DESCRIPTOR_HANDLE(m_cbvHeap->GetGPUDescriptorHandleForHeapStart(), 
            pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)*accessorIndex));
    }
};

struct Timer 
{
    LARGE_INTEGER frequency;
    LARGE_INTEGER start;

    void Init() 
    {
        QueryPerformanceFrequency(&frequency);
        QueryPerformanceCounter(&start);
    }

    double GetElapsedTimeInSeconds() 
    {
        LARGE_INTEGER end;
        QueryPerformanceCounter(&end);
        return static_cast<double>(end.QuadPart - start.QuadPart) / frequency.QuadPart;
    }
};



class Renderer : public DXSample
{
public:
    Renderer(UINT width, UINT height, std::wstring name);

    virtual void OnInit();
    virtual void OnUpdate();
    virtual void OnRender();
    virtual void OnDestroy();

private:
    // In this sample we overload the meaning of FrameCount to mean both the maximum
    // number of frames that will be queued to the GPU at a time, as well as the number
    // of back buffers in the DXGI swap chain. For the majority of applications, this
    // is convenient and works well. However, there will be certain cases where an
    // application may want to queue up more frames than there are back buffers
    // available.
    // It should be noted that excessive buffering of frames dependent on user input
    // may result in noticeable latency in your app.
    static const UINT FrameCount = 3;
    static const UINT TextureWidth = 256;
    static const UINT TextureHeight = 256;
    static const UINT TexturePixelSize = 4;    // The number of bytes used to represent a pixel in the texture.


    //ST_Matrix4 mvp;

    struct alignas(256) SceneConstantBuffer
    {
        ST_Matrix4 mvp;
    };
   // static_assert((sizeof(SceneConstantBuffer) % 256) == 0, "Constant Buffer size must be 256-byte aligned");

    // Pipeline objects.
    CD3DX12_VIEWPORT m_viewport;
    CD3DX12_RECT m_scissorRect;
    ComPtr<IDXGISwapChain3> m_swapChain;
    ComPtr<ID3D12Device> m_device;
    ComPtr<ID3D12Resource> m_renderTargets[FrameCount];
    ComPtr<ID3D12CommandAllocator> m_commandAllocators[FrameCount];
    ComPtr<ID3D12CommandQueue> m_commandQueue;
    //ComPtr<ID3D12RootSignature> m_rootSignature;
    RootSigniture mRootSigniture;
    ComPtr<ID3D12DescriptorHeap> m_rtvHeap;
    ComPtr<ID3D12DescriptorHeap> m_cbvHeap;

   // ComPtr<ID3D12DescriptorHeap> m_srvHeap;
    ComPtr<ID3D12DescriptorHeap> m_dsvHeap;
    //ComPtr<ID3D12PipelineState> m_pipelineState;
    Pipeline mPipeline;
    ComPtr<ID3D12GraphicsCommandList> m_commandList;
    UINT m_rtvDescriptorSize;

    // App resources.

    //ComPtr<ID3D12Resource> m_constantBuffer;
    //UINT8* m_pCbvDataBegin;
    SceneConstantBuffer m_constantBufferData;
    ComPtr<ID3D12Resource> m_texture;
    ComPtr<ID3D12Resource> m_depthStencil;
    VertexBuffer mCubeVB;
    ConstantBufferAccessor mConstantBufferAccessors[500];

    // Synchronization objects.
    UINT m_frameIndex;
    HANDLE m_fenceEvent;
    ComPtr<ID3D12Fence> m_fence;
    UINT64 m_fenceValues[FrameCount];

    Timer timer;

    void LoadPipeline();
    void LoadAssets();
    void PopulateCommandList();
    void MoveToNextFrame();
    void WaitForGpu();
};
