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
#include "DescriptorHandleProvider.h"
#include "ConstantBuffer.h"
#include "VertexBuffer.h"
#include "Time.h"
#include "Camera.h"
#include "Texture.h"
#include "Input.h"
#include "Scene.h"

class Renderer : public DXSample
{
public:
    Renderer(UINT width, UINT height, std::wstring name);

    virtual void OnInit();
    virtual void OnUpdate();
    virtual void OnRender();
    virtual void OnDestroy();

    //public resources
    Texture texture;
    VertexBuffer mCubeVB;
    VertexBuffer mCubeWireFrameVB;
    VertexBuffer mPlaneVB;
    VertexBuffer mSphereWireFrameVB;
    VertexBuffer mSphereVB;
    VertexBuffer mGridVB;
    VertexBuffer mLineVB;
    VertexBuffer mCylinderVB;

    struct alignas(256) SceneConstantBuffer
    {
        ST_Matrix4 mvp;
        ST_Matrix4 model;
    };

    struct PixelShaderConstantBuffer
    {
        ST_Vector4 cameraPos;
        ST_Vector4 lightDir;
        ST_Vector4 lightColor;
        ST_Vector4 color;
        float colorMix;
    };

    SceneConstantBuffer m_constantBufferData;
    PixelShaderConstantBuffer pixelShaderConstantBuffer;

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


   // static_assert((sizeof(SceneConstantBuffer) % 256) == 0, "Constant Buffer size must be 256-byte aligned");

    // Pipeline objects.
    CD3DX12_VIEWPORT m_viewport;
    CD3DX12_RECT m_scissorRect;
    ComPtr<IDXGISwapChain3> m_swapChain;
    ComPtr<ID3D12Device> m_device;
    ComPtr<ID3D12Resource> m_renderTargets[FrameCount];
    ComPtr<ID3D12CommandAllocator> m_commandAllocators[FrameCount];
    ComPtr<ID3D12CommandQueue> m_commandQueue;
    RootSigniture mRootSigniture;
    RootSigniture mRootSignitureWireFrame;
    ComPtr<ID3D12DescriptorHeap> m_rtvHeap;
    DescriptorHandleProvider dhp;
    ComPtr<ID3D12DescriptorHeap> m_dsvHeap;
    Pipeline mPipeline;
    Pipeline mPipelineWireFrame;
    ComPtr<ID3D12GraphicsCommandList> m_commandList;
    UINT m_rtvDescriptorSize;

    // App resources.

    //ComPtr<ID3D12Resource> m_constantBuffer;
    //UINT8* m_pCbvDataBegin;

    
    ComPtr<ID3D12Resource> m_depthStencil;

    //ConstantBufferAccessor mConstantBufferAccessors[500];
    ConstantBufferAccessorStack cbaStack;
    ST_Matrix4 projection;

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

public:

    static Renderer instance;

    Scene scene;
    // Draw methods
    void drawPrimitive(ST_Vector3 position, ST_Quaternion rotation, ST_Vector3 scale, Texture& texture, PrimitiveType type);
    void drawPrimitive(ST_Vector3 position, ST_Quaternion rotation, ST_Vector3 scale, ST_Vector4 color, PrimitiveType type);
    void drawPrimitive(ST_Vector3 position, ST_Quaternion rotation, ST_Vector3 scale, ST_Vector4 color, Texture& texture, float colorMix, PrimitiveType type);
    void drawWireFrame(ST_Vector3 position, ST_Quaternion rotation, ST_Vector3 scale, ST_Vector4 color, PrimitiveType type);
    void drawLine(const ST_Vector3& from, const ST_Vector3& to, const ST_Vector4& color);
};


