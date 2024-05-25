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
#include "Model.h"
#include "Input.h"
#include "Scene.h"


class Renderer : public DXSample
{
    friend class Scene;
public:
    Renderer(UINT width, UINT height, std::wstring name);

    virtual void OnInit();
    virtual void OnUpdate();
    virtual void OnRender();
    virtual void OnDestroy();

    //public resources
    Texture checkers;
    Texture tile;
    VertexBuffer mCubeVB;
    VertexBuffer mCubeWireFrameVB;
    VertexBuffer mPlaneVB;
    VertexBuffer mSphereWireFrameVB;
    VertexBuffer mSphereVB;
    VertexBuffer mGridVB;
    VertexBuffer mLineVB;
    VertexBuffer mCylinderVB;
    VertexBuffer mMonkeyVB;

   // Model sponza;

    struct alignas(256) VertexShaderConstantBuffer
    {
        ST_Matrix4 mvp;
        ST_Matrix4 model;
        ST_Matrix4 lightViewProjection;
        ST_Vector4 color;
        float colorMix;
    };

    struct alignas(256) VertexShaderInstancedConstantBuffer
    {
        ST_Matrix4 mvp[400];
        ST_Matrix4 model[400];
        ST_Vector4 colors[400];
        ST_Matrix4 lightViewProj;
    };

    struct alignas(256) WireFrameInstancedConstantBuffer
    {
        ST_Matrix4 viewProjection;
        ST_Matrix4 lightViewProj;
        ST_Matrix4 model[400];
        ST_Vector4 colors[400];
    };

    struct alignas(256) VertexShaderInstancedConstantBufferShadows
    {
        ST_Matrix4 mvp[400];
    };

    struct alignas(256) PixelShaderConstantBuffer
    {
        ST_Vector4 cameraPos;
        ST_Vector4 lightDir;
        ST_Vector4 lightColor;
    };

    VertexShaderConstantBuffer m_constantBufferData;
    PixelShaderConstantBuffer pixelShaderConstantBuffer;
    ConstantBufferAccessor pixelShaderConstantBufferAccessor;

    Camera mainCamera;
    Camera directionalLightCamera;
    ST_Vector3 dirLightOffset;
    ST_Matrix4 lightViewProjection;

    UINT perPrimitiveInstanceBufferCounts[4] = { 0,0,0,0 };
    VertexShaderInstancedConstantBuffer perPrimitiveInstanceBuffer[4];
    ConstantBufferAccessor perPrimitiveInstanceCBAAccessors[4];
    UINT perWireFramePrimitiveInstanceBufferCounts[4] = { 0,0,0,0 };
    UINT perWireFramePrimitiveInstanceBufferCapacities[4] = { 400,400,400,400 };
    std::vector<WireFrameInstancedConstantBuffer> perWireFramePrimitiveInstanceBuffers[4];
    std::vector<ConstantBufferAccessor> perWireFramePrimitiveInstanceCBAAccessors[4];
    UINT perPrimitiveInstanceBufferCountsShadows[4] = { 0,0,0,0 };
    VertexShaderInstancedConstantBufferShadows perPrimitiveInstanceBufferShadows[4];
    ConstantBufferAccessor perPrimitiveInstanceCBAAccessorsShadows[4];


private:
    // In this sample we overload the meaning of FrameCount to mean both the maximum
    // number of frames that will be queued to the GPU at a time, as well as the number
    // of back buffers in the DXGI swap chain. For the majority of applications, this
    // is convenient and works well. However, there will be certain cases where an
    // application may want to queue up more frames than there are back buffers
    // available.
    // It should be noted that excessive buffering of frames dependent on user input
    // may result in noticeable latency in your app.
    static const UINT FrameCount = 10;
    static const UINT TextureWidth = 256;
    static const UINT TextureHeight = 256;
    static const UINT TexturePixelSize = 4;    // The number of bytes used to represent a pixel in the texture.


    //ST_Matrix4 mvp;


   // static_assert((sizeof(VertexShaderConstantBuffer) % 256) == 0, "Constant Buffer size must be 256-byte aligned");

    // Pipeline objects.
    CD3DX12_VIEWPORT m_viewport;
    CD3DX12_RECT m_scissorRect;
    ComPtr<IDXGISwapChain3> m_swapChain;
    ComPtr<ID3D12Device> m_device;
    ComPtr<ID3D12Resource> m_renderTargets[FrameCount];
    ComPtr<ID3D12CommandAllocator> m_commandAllocators[FrameCount];
    ComPtr<ID3D12CommandQueue> m_commandQueue;
    RootSigniture mRootSigniture;
    Pipeline mPipeline;
    RootSigniture mRootSignitureShadow;
    Pipeline mPipelineShadow;
    Pipeline mPipelineShadowInstanced;
    RootSigniture mRootSignitureInstanced;
    Pipeline mPipelineInstanced;
    RootSigniture mRootSignitureWireFrame;
    Pipeline mPipelineWireFrame;
    ComPtr<ID3D12DescriptorHeap> m_rtvHeap;
    DescriptorHandleProvider dhp;
    ComPtr<ID3D12DescriptorHeap> m_dsvHeap;
    ComPtr<ID3D12GraphicsCommandList> m_commandList;
    UINT m_rtvDescriptorSize;

    // App resources.

    //ComPtr<ID3D12Resource> m_constantBuffer;
    //UINT8* m_pCbvDataBegin;

    
    ComPtr<ID3D12Resource> m_depthStencil;
    ComPtr<ID3D12Resource> shadowDepthBuffer;
    const UINT shadowMapWidth = 1024;
    const UINT shadowMapHeight = 1024;
    D3D12_CPU_DESCRIPTOR_HANDLE shadowSrvCpuHandle;
    D3D12_GPU_DESCRIPTOR_HANDLE shadowSrvGpuHandle;
    bool isShadowPass = false;

    //ConstantBufferAccessor mConstantBufferAccessors[500];
    ConstantBufferAccessorStack cbaStack;
    

    // Synchronization objects.
    UINT m_frameIndex;
    HANDLE m_fenceEvent;
    ComPtr<ID3D12Fence> m_fence;
    UINT64 m_fenceValues[FrameCount];

    Time timer;

    ComPtr<ID3D12Debug> debugController;

    void LoadPipeline();
    void LoadAssets();
    void PopulateCommandList();
    void MoveToNextFrame();
    void WaitForGpu();
    void writeShadowDepthBufferToDDS();

public:

    static Renderer instance;

    Scene* pScene;
    // Draw methods
    void drawPrimitive(ST_Vector3 position, ST_Quaternion rotation, ST_Vector3 scale, Texture& texture, PrimitiveType type);
    void drawPrimitive(ST_Vector3 position, ST_Quaternion rotation, ST_Vector3 scale, ST_Vector4 color, PrimitiveType type);
    void drawPrimitive(ST_Vector3 position, ST_Quaternion rotation, ST_Vector3 scale, ST_Vector4 color, Texture& texture, float colorMix, PrimitiveType type);
    void drawVertexBuffer(ST_Vector3 position, ST_Quaternion rotation, ST_Vector3 scale, ST_Vector4 color, Texture& texture, float colorMix, VertexBuffer& vertexBuffer);
    void drawModel(ST_Vector3 position, ST_Quaternion rotation, ST_Vector3 scale, ST_Vector4 color, float colorMix, Model& model);
    void addPrimitiveInstance(ST_Vector3 position, ST_Quaternion rotation, ST_Vector3 scale, ST_Vector4 color, PrimitiveType type);
    void addPrimitiveInstance(ST_Vector3 position, ST_Quaternion rotation, ST_Vector3 scale, PrimitiveType type);
    void drawAddedPrimitiveInstances();
    void drawWireFrame(ST_Vector3 position, ST_Quaternion rotation, ST_Vector3 scale, ST_Vector4 color, PrimitiveType type);
    void addWireFrameInstance(ST_Vector3 position, ST_Quaternion rotation, ST_Vector3 scale, ST_Vector4 color, PrimitiveType type);
    void drawAddedWireFrameInstances();
    void drawLine(const ST_Vector3& from, const ST_Vector3& to, const ST_Vector4& color);
};


