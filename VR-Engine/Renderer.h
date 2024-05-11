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

#include "Pipeline.h"
#include "VertexBuffer.h"
#include "DescriptorHandleProvider.h"
#include "ConstantBuffer.h"
#include "Time.h"




//struct Timer 
//{
//    LARGE_INTEGER frequency;
//    LARGE_INTEGER start;
//
//    void Init() 
//    {
//        QueryPerformanceFrequency(&frequency);
//        QueryPerformanceCounter(&start);
//    }
//
//    double GetElapsedTimeInSeconds() 
//    {
//        LARGE_INTEGER end;
//        QueryPerformanceCounter(&end);
//        return static_cast<double>(end.QuadPart - start.QuadPart) / frequency.QuadPart;
//    }
//};

struct Texture
{
    ID3D12Resource* m_texture;
    D3D12_GPU_DESCRIPTOR_HANDLE gpuDescriptorHandle;

    void init(ID3D12Device* pDevice, ID3D12GraphicsCommandList* pCommandList, ID3D12Resource* textureUploadHeap, DescriptorHandleProvider& dhp)
    {
        // Create the texture.
        {
            // Describe and create a Texture2D.
            D3D12_RESOURCE_DESC textureDesc = {};
            textureDesc.MipLevels = 1;
            textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
            textureDesc.Width = 2;
            textureDesc.Height = 2;
            textureDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
            textureDesc.DepthOrArraySize = 1;
            textureDesc.SampleDesc.Count = 1;
            textureDesc.SampleDesc.Quality = 0;
            textureDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;

            CD3DX12_HEAP_PROPERTIES heapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
            ThrowIfFailed(pDevice->CreateCommittedResource(
                &heapProps,
                D3D12_HEAP_FLAG_NONE,
                &textureDesc,
                D3D12_RESOURCE_STATE_COPY_DEST,
                nullptr,
                IID_PPV_ARGS(&m_texture)));

            const UINT64 uploadBufferSize = GetRequiredIntermediateSize(m_texture, 0, 1);

            // Create the GPU upload buffer.
            heapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
            CD3DX12_RESOURCE_DESC resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(uploadBufferSize);
            ThrowIfFailed(pDevice->CreateCommittedResource(
                &heapProps,
                D3D12_HEAP_FLAG_NONE,
                &resourceDesc,
                D3D12_RESOURCE_STATE_GENERIC_READ,
                nullptr,
                IID_PPV_ARGS(&textureUploadHeap)));

            // Copy data to the intermediate upload heap and then schedule a copy 
            // from the upload heap to the Texture2D.
            std::vector<UINT8> texture = {
                255, 255, 255, 255,
                0,  0, 0, 255,
                0, 0, 0, 255,
                255, 255, 255, 255
            };

            D3D12_SUBRESOURCE_DATA textureData = {};
            textureData.pData = &texture[0];
            textureData.RowPitch = 2 * 4;
            textureData.SlicePitch = textureData.RowPitch * 2;

            UpdateSubresources(pCommandList, m_texture, textureUploadHeap, 0, 0, 1, &textureData);
            CD3DX12_RESOURCE_BARRIER resourceBarrier = CD3DX12_RESOURCE_BARRIER::Transition(m_texture, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
            pCommandList->ResourceBarrier(1, &resourceBarrier);

            // Describe and create a SRV for the texture.
            D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
            srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
            srvDesc.Format = textureDesc.Format;
            srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
            srvDesc.Texture2D.MipLevels = 1;
            D3D12_CPU_DESCRIPTOR_HANDLE handle = dhp.getCpuHandle(pDevice);
            pDevice->CreateShaderResourceView(m_texture, &srvDesc, handle);
            gpuDescriptorHandle = dhp.GpuHandleFromCpuHandle(handle);
        }
    }

    void bind(ID3D12GraphicsCommandList* pCommandList, UINT rootParameterIndex)
    {
        pCommandList->SetGraphicsRootDescriptorTable(rootParameterIndex, gpuDescriptorHandle);
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
    //ComPtr<ID3D12DescriptorHeap> m_cbvHeap;
    DescriptorHandleProvider dhp;
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
    Texture texture;
    
    ComPtr<ID3D12Resource> m_depthStencil;
    VertexBuffer mCubeVB;
    VertexBuffer mPlaneVB;
    VertexBuffer mSphereVB;
    ConstantBufferAccessor mConstantBufferAccessors[500];

    // Synchronization objects.
    UINT m_frameIndex;
    HANDLE m_fenceEvent;
    ComPtr<ID3D12Fence> m_fence;
    UINT64 m_fenceValues[FrameCount];

    Time timer;

    void LoadPipeline();
    void LoadAssets();
    void PopulateCommandList();
    void MoveToNextFrame();
    void WaitForGpu();
};
