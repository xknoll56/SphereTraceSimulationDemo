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

#include "stdafx.h"
#include "Renderer.h"

Renderer Renderer::instance(1280, 720, L"D3D12");
extern HWND hwnd;

Renderer::Renderer(UINT width, UINT height, std::wstring name) :
    DXSample(width, height, name),
    m_frameIndex(0),
    m_viewport(0.0f, 0.0f, static_cast<float>(width), static_cast<float>(height)),
    m_scissorRect(0, 0, static_cast<LONG>(width), static_cast<LONG>(height)),
    m_fenceValues{},
    m_rtvDescriptorSize(0)
{
}

void Renderer::OnInit()
{
    LoadPipeline();
    LoadAssets();
}

// Load the rendering pipeline dependencies.
void Renderer::LoadPipeline()
{
    UINT dxgiFactoryFlags = 0;

#if defined(_DEBUG)
    // Enable the debug layer (requires the Graphics Tools "optional feature").
    // NOTE: Enabling the debug layer after device creation will invalidate the active device.
    {
        ComPtr<ID3D12Debug> debugController;
        if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
        {
            debugController->EnableDebugLayer();

            // Enable additional debug layers.
            dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
        }
    }
#endif

    ComPtr<IDXGIFactory4> factory;
    ThrowIfFailed(CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&factory)));

    if (m_useWarpDevice)
    {
        ComPtr<IDXGIAdapter> warpAdapter;
        ThrowIfFailed(factory->EnumWarpAdapter(IID_PPV_ARGS(&warpAdapter)));

        ThrowIfFailed(D3D12CreateDevice(
            warpAdapter.Get(),
            D3D_FEATURE_LEVEL_11_0,
            IID_PPV_ARGS(&m_device)
            ));
    }
    else
    {
        ComPtr<IDXGIAdapter1> hardwareAdapter;
        GetHardwareAdapter(factory.Get(), &hardwareAdapter);

        ThrowIfFailed(D3D12CreateDevice(
            hardwareAdapter.Get(),
            D3D_FEATURE_LEVEL_11_0,
            IID_PPV_ARGS(&m_device)
            ));
    }

    // Describe and create the command queue.
    D3D12_COMMAND_QUEUE_DESC queueDesc = {};
    queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

    ThrowIfFailed(m_device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_commandQueue)));

    // Describe and create the swap chain.
    DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
    swapChainDesc.BufferCount = FrameCount;
    swapChainDesc.Width = m_width;
    swapChainDesc.Height = m_height;
    swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    swapChainDesc.SampleDesc.Count = 1;

    ComPtr<IDXGISwapChain1> swapChain;
    ThrowIfFailed(factory->CreateSwapChainForHwnd(
        m_commandQueue.Get(),        // Swap chain needs the queue so that it can force a flush on it.
        hwnd,
        &swapChainDesc,
        nullptr,
        nullptr,
        &swapChain
        ));

    // This sample does not support fullscreen transitions.
    ThrowIfFailed(factory->MakeWindowAssociation(hwnd, DXGI_MWA_NO_ALT_ENTER));

    ThrowIfFailed(swapChain.As(&m_swapChain));
    m_frameIndex = m_swapChain->GetCurrentBackBufferIndex();

    // Create descriptor heaps.
    {
        // Describe and create a render target view (RTV) descriptor heap.
        D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
        rtvHeapDesc.NumDescriptors = FrameCount;
        rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
        rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
        ThrowIfFailed(m_device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&m_rtvHeap)));


        // Describe and create a constant buffer view (CBV) descriptor heap.
        // Flags indicate that this descriptor heap can be bound to the pipeline 
        // and that descriptors contained in it can be referenced by a root table.
        //D3D12_DESCRIPTOR_HEAP_DESC cbvHeapDesc = {};
        //cbvHeapDesc.NumDescriptors = 1000;
        //cbvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
        //cbvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
        //ThrowIfFailed(m_device->CreateDescriptorHeap(&cbvHeapDesc, IID_PPV_ARGS(&m_cbvHeap)));
        dhp.init(m_device.Get(), 10000);

        //// Describe and create a shader resource view (SRV) heap for the texture.
        //D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
        //srvHeapDesc.NumDescriptors = 1;
        //srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
        //srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
        //ThrowIfFailed(m_device->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&m_srvHeap)));

        // Step 1: Define Descriptor Heap Parameters
        D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc = {};
        dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV; // Type of descriptors (Depth Stencil Views)
        dsvHeapDesc.NumDescriptors = 2; // Number of descriptors (usually one for depth buffer)
        dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE; // No special flags needed
        ThrowIfFailed(m_device->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&m_dsvHeap)));

        m_rtvDescriptorSize = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
    }

    // Create frame resources.
    {
        CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_rtvHeap->GetCPUDescriptorHandleForHeapStart());

        // Create a RTV and a command allocator for each frame.
        for (UINT n = 0; n < FrameCount; n++)
        {
            ThrowIfFailed(m_swapChain->GetBuffer(n, IID_PPV_ARGS(&m_renderTargets[n])));
            m_device->CreateRenderTargetView(m_renderTargets[n].Get(), nullptr, rtvHandle);
            rtvHandle.Offset(1, m_rtvDescriptorSize);

            ThrowIfFailed(m_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_commandAllocators[n])));
        }
    }

    timer.init();
}





// Load the sample assets.
void Renderer::LoadAssets()
{


    // Create a root signature consisting of a descriptor table with a single CBV.
    {

         CD3DX12_DESCRIPTOR_RANGE1 ranges[4];
         CD3DX12_ROOT_PARAMETER1 rootParameters[4];
         ranges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC);
         ranges[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 1, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC);
         ranges[2].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC);
         ranges[3].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 1);
         rootParameters[0].InitAsDescriptorTable(1, &ranges[0], D3D12_SHADER_VISIBILITY_VERTEX);
         rootParameters[1].InitAsDescriptorTable(1, &ranges[1], D3D12_SHADER_VISIBILITY_PIXEL);
         rootParameters[2].InitAsDescriptorTable(1, &ranges[2], D3D12_SHADER_VISIBILITY_PIXEL);
         rootParameters[3].InitAsDescriptorTable(1, &ranges[3], D3D12_SHADER_VISIBILITY_PIXEL);

        D3D12_STATIC_SAMPLER_DESC sampler = {};
        sampler.Filter = D3D12_FILTER_MIN_MAG_MIP_POINT;
        sampler.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
        sampler.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
        sampler.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
        sampler.MipLODBias = 0;
        sampler.MaxAnisotropy = 0;
        sampler.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
        sampler.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
        sampler.MinLOD = 0.0f;
        sampler.MaxLOD = D3D12_FLOAT32_MAX;
        sampler.ShaderRegister = 0;
        sampler.RegisterSpace = 0;
        sampler.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

        D3D12_STATIC_SAMPLER_DESC staticSamplerDesc = {};
        staticSamplerDesc.Filter = D3D12_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;
        staticSamplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
        staticSamplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
        staticSamplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
        staticSamplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL; // Specify the comparison function
        staticSamplerDesc.BorderColor = D3D12_STATIC_BORDER_COLOR_OPAQUE_BLACK;
        staticSamplerDesc.MinLOD = 0.0f;
        staticSamplerDesc.MaxLOD = D3D12_FLOAT32_MAX;
        staticSamplerDesc.ShaderRegister =1;
        staticSamplerDesc.RegisterSpace = 0;
        staticSamplerDesc.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; // Set the shader visibility as needed

        D3D12_STATIC_SAMPLER_DESC samplers[2] =
        {
            sampler,
            staticSamplerDesc
        };


        D3D12_ROOT_SIGNATURE_FLAGS rootSignatureFlags =
            D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
        // D3D12_ROOT_SIGNATURE_FLAGS rootSignatureFlags =
        //D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
        //D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
        //D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
        //D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS |
        //D3D12_ROOT_SIGNATURE_FLAG_DENY_PIXEL_SHADER_ROOT_ACCESS;

        //CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc;
        //rootSignatureDesc.Init_1_1(2, rootParameters, 1, &sampler, rootSignatureFlags);

        mRootSigniture.init(m_device.Get(), rootParameters, 4, rootSignatureFlags, samplers, 2);
    }

    // Create the pipeline state, which includes compiling and loading shaders.
    {
        // Define the vertex input layout.
        D3D12_INPUT_ELEMENT_DESC inputElementDescs[] =
        {
            { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
            { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
            { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
        };


        mPipeline.init(m_device.Get(), mRootSigniture, L"DefaultShaders.hlsl", L"DefaultShaders.hlsl", inputElementDescs, _countof(inputElementDescs), D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);
    }

    // Create the root signiture for the wire frame pipeline
    {

        CD3DX12_DESCRIPTOR_RANGE1 ranges[3];
        CD3DX12_ROOT_PARAMETER1 rootParameters[3];

        ranges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC);
        ranges[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 1, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC);
        ranges[2].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);
        rootParameters[0].InitAsDescriptorTable(1, &ranges[0], D3D12_SHADER_VISIBILITY_VERTEX);
        rootParameters[1].InitAsDescriptorTable(1, &ranges[1], D3D12_SHADER_VISIBILITY_PIXEL);
        rootParameters[2].InitAsDescriptorTable(1, &ranges[2], D3D12_SHADER_VISIBILITY_PIXEL);

        // Allow input layout and deny uneccessary access to certain pipeline stages.
        D3D12_ROOT_SIGNATURE_FLAGS rootSignatureFlags =
            D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

        D3D12_STATIC_SAMPLER_DESC staticSamplerDesc = {};
        staticSamplerDesc.Filter = D3D12_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;
        staticSamplerDesc.AddressU = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
        staticSamplerDesc.AddressV = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
        staticSamplerDesc.AddressW = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
        staticSamplerDesc.ComparisonFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL; // Specify the comparison function
        staticSamplerDesc.BorderColor = D3D12_STATIC_BORDER_COLOR_OPAQUE_BLACK;
        staticSamplerDesc.MinLOD = 0.0f;
        staticSamplerDesc.MaxLOD = D3D12_FLOAT32_MAX;
        staticSamplerDesc.ShaderRegister = 0;
        staticSamplerDesc.RegisterSpace = 0;
        staticSamplerDesc.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL; // Set the shader visibility as needed

        mRootSignitureInstanced.init(m_device.Get(), rootParameters, 3, rootSignatureFlags, &staticSamplerDesc, 1);
    }

    // Create the pipeline state, which includes compiling and loading shaders.
    {
        // Define the vertex input layout.
        D3D12_INPUT_ELEMENT_DESC inputElementDescs[] =
        {
            { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
            { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
            { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
        };


        mPipelineInstanced.init(m_device.Get(), mRootSignitureInstanced, L"DefaultInstancedShaders.hlsl", L"DefaultInstancedShaders.hlsl", inputElementDescs, _countof(inputElementDescs), D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);
    }

    // Create the root signiture for the wire frame pipeline
    {

        CD3DX12_DESCRIPTOR_RANGE1 ranges[2];
        CD3DX12_ROOT_PARAMETER1 rootParameters[2];

        ranges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC);
        rootParameters[0].InitAsDescriptorTable(1, &ranges[0], D3D12_SHADER_VISIBILITY_VERTEX);

        // Allow input layout and deny uneccessary access to certain pipeline stages.
        D3D12_ROOT_SIGNATURE_FLAGS rootSignatureFlags =
            D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

        mRootSignitureWireFrame.init(m_device.Get(), rootParameters, 1, rootSignatureFlags);
    }


    // Create the pipeline for the wireframe models
    {
        // Define the vertex input layout.
        D3D12_INPUT_ELEMENT_DESC inputElementDescs[] =
        {
            { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
        };


        mPipelineWireFrame.init(m_device.Get(), mRootSignitureWireFrame, L"WireFrameShaders.hlsl", L"WireFrameShaders.hlsl", inputElementDescs, _countof(inputElementDescs),
            D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE);
    }

    // Create the root signiture for the shadow pipeline
    {

        CD3DX12_DESCRIPTOR_RANGE1 ranges[1];
        CD3DX12_ROOT_PARAMETER1 rootParameters[1];

        ranges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC);
        rootParameters[0].InitAsDescriptorTable(1, &ranges[0], D3D12_SHADER_VISIBILITY_VERTEX);

        // Allow input layout and deny uneccessary access to certain pipeline stages.
        D3D12_ROOT_SIGNATURE_FLAGS rootSignatureFlags =
            D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

        mRootSignitureShadow.init(m_device.Get(), rootParameters, 1, rootSignatureFlags);
    }


    // Create the pipeline for the shadow models
    {
        // Define the vertex input layout.
        D3D12_INPUT_ELEMENT_DESC inputElementDescs[] =
        {
            { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
        };


        mPipelineShadow.initShadows(m_device.Get(), mRootSignitureShadow, L"ShadowPass.hlsl", L"ShadowPass.hlsl", inputElementDescs, _countof(inputElementDescs),
            D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);
    }

    // Create the pipeline for the shadow models
    {
        // Define the vertex input layout.
        D3D12_INPUT_ELEMENT_DESC inputElementDescs[] =
        {
            { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
        };


        mPipelineShadowInstanced.initShadows(m_device.Get(), mRootSignitureShadow, L"ShadowPassInstanced.hlsl", L"ShadowPassInstanced.hlsl", inputElementDescs, _countof(inputElementDescs),
            D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);
    }

    // Create the command list.
    ThrowIfFailed(m_device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_commandAllocators[m_frameIndex].Get(), mPipeline.pPipelineState, IID_PPV_ARGS(&m_commandList)));



    mCubeVB = VertexBuffer::createCube(m_device.Get());
    mCubeWireFrameVB = VertexBuffer::createCubeWireFrame(m_device.Get());
    mPlaneVB = VertexBuffer::createPlane(m_device.Get());
    mSphereVB = VertexBuffer::createSphere(m_device.Get());
    mGridVB = VertexBuffer::createGrid(m_device.Get());
    mSphereWireFrameVB = VertexBuffer::createSphereWireFrame(m_device.Get());
    mLineVB = VertexBuffer::createLine(m_device.Get());
    mCylinderVB = VertexBuffer::createCylinder(m_device.Get());

    mMonkeyVB = VertexBuffer::readFromObj(m_device.Get(), "Models/monkey.obj", nullptr);


    

    //create constant buffer
    //for(int i = 0; i<400; i++)
    //    mConstantBufferAccessors[i].init(m_device.Get(), dhp, &m_constantBufferData, sizeof(VertexShaderConstantBuffer));
    //mConstantBufferAccessors[400].init(m_device.Get(), dhp, &m_constantBufferData, sizeof(VertexShaderConstantBuffer));
    cbaStack = ConstantBufferAccessorStack(3);
    pixelShaderConstantBufferAccessor.init(m_device.Get(), dhp, &pixelShaderConstantBuffer, sizeof(pixelShaderConstantBuffer));
    ConstantBufferAccessor accessor;
    for (int i = 0; i < 4; i++)
    {
        perPrimitiveInstanceBuffers[i].push_back(instanceBuf);
        perPrimitiveInstanceCBAAccessors[i].push_back(accessor);
        perPrimitiveInstanceCBAAccessors[i][0].init(m_device.Get(), dhp, &perPrimitiveInstanceBuffers[0][0], sizeof(WireFrameInstancedConstantBuffer));

        perWireFramePrimitiveInstanceBuffers[i].push_back(instanceBuf);
        perWireFramePrimitiveInstanceCBAAccessors[i].push_back(accessor);
        perWireFramePrimitiveInstanceCBAAccessors[i][0].init(m_device.Get(), dhp, &perPrimitiveInstanceBuffers[0][0], sizeof(WireFrameInstancedConstantBuffer));

        perPrimitiveInstanceBufferShadows[i].push_back(shadowInstanceBuf);
        perPrimitiveInstanceCBAAccessorsShadows[i].push_back(accessor);
        perPrimitiveInstanceCBAAccessorsShadows[i][0].init(m_device.Get(), dhp, &perPrimitiveInstanceBufferShadows[0], sizeof(VertexShaderInstancedConstantBufferShadows));
    }
    UINT cbastackhandle = cbaStack.createStack(m_device.Get(), dhp, sizeof(VertexShaderConstantBuffer), 2000);
    UINT cbastackhandle1 = cbaStack.createStack(m_device.Get(), dhp, 256, 2000);
    cbaStack.createStack(m_device.Get(), dhp, 256, 2000);


    // Note: ComPtr's are CPU objects but this resource needs to stay in scope until
      // the command list that references it has finished executing on the GPU.
      // We will flush the GPU at the end of this method to ensure the resource is not
      // prematurely destroyed.

    ComPtr<ID3D12Resource> textureUploadHeap;

    // Copy data to the intermediate upload heap and then schedule a copy 
// from the upload heap to the Texture2D.
    unsigned char texBytes[] = {
        255, 255, 255, 255,
        50,  50, 50, 255,
        50, 50, 50, 255,
        255, 255, 255, 255
    };

    checkers.init(m_device.Get(), m_commandList.Get(),textureUploadHeap.Get(), dhp,
        2, 2, texBytes);
    
    tile.init(m_device.Get(), m_commandList.Get(), textureUploadHeap.Get(), dhp, "Textures/tile360.png");

    ComPtr<ID3D12Resource> textureUploadHeap1;
    //sponza = Model::modelFromObjFile(Renderer::instance.m_device.Get(), Renderer::instance.m_commandList.Get(), textureUploadHeap1.Get(), Renderer::instance.dhp,
    //    "Models/Sponza/sponza.mtl", "Models/Sponza/sponza.obj");

    //create dsv
    // Create the depth stencil view.
    {
        D3D12_DEPTH_STENCIL_VIEW_DESC depthStencilDesc = {};
        depthStencilDesc.Format = DXGI_FORMAT_D32_FLOAT;
        depthStencilDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
        depthStencilDesc.Flags = D3D12_DSV_FLAG_NONE;

        D3D12_CLEAR_VALUE depthOptimizedClearValue = {};
        depthOptimizedClearValue.Format = DXGI_FORMAT_D32_FLOAT;
        depthOptimizedClearValue.DepthStencil.Depth = 1.0f;
        depthOptimizedClearValue.DepthStencil.Stencil = 0;

        CD3DX12_HEAP_PROPERTIES heapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
        CD3DX12_RESOURCE_DESC resourceDesc = CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_D32_FLOAT, m_width, m_height, 1, 0, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL);
        ThrowIfFailed(m_device->CreateCommittedResource(
            &heapProps,
            D3D12_HEAP_FLAG_NONE,
            &resourceDesc,
            D3D12_RESOURCE_STATE_DEPTH_WRITE,
            &depthOptimizedClearValue,
            IID_PPV_ARGS(&m_depthStencil)
        ));

        NAME_D3D12_OBJECT(m_depthStencil);

        m_device->CreateDepthStencilView(m_depthStencil.Get(), &depthStencilDesc, m_dsvHeap->GetCPUDescriptorHandleForHeapStart());


        resourceDesc = CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_D32_FLOAT, shadowMapWidth, shadowMapHeight, 1, 0, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL);
        ThrowIfFailed(m_device->CreateCommittedResource(
            &heapProps,
            D3D12_HEAP_FLAG_NONE,
            &resourceDesc,
            D3D12_RESOURCE_STATE_DEPTH_WRITE,
            &depthOptimizedClearValue,
            IID_PPV_ARGS(&shadowDepthBuffer)
        ));



        D3D12_CPU_DESCRIPTOR_HANDLE handle = m_dsvHeap->GetCPUDescriptorHandleForHeapStart();
        handle.ptr += m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
        m_device->CreateDepthStencilView(shadowDepthBuffer.Get(), nullptr, handle);

        D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
        srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
        srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
        srvDesc.Texture2D.MostDetailedMip = 0;
        srvDesc.Texture2D.MipLevels = 1;
        srvDesc.Texture2D.PlaneSlice = 0;
        srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;
        srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;

        shadowSrvCpuHandle = dhp.getCpuHandle(m_device.Get());
        m_device->CreateShaderResourceView(shadowDepthBuffer.Get(), &srvDesc, shadowSrvCpuHandle);
        shadowSrvGpuHandle = dhp.GpuHandleFromCpuHandle(shadowSrvCpuHandle);
    }


    dirLightOffset = ST_VECTOR3(0, 50, 0);
    directionalLightCamera = Camera::cameraConstruct(dirLightOffset, M_PI * 0.50f, -M_PI * 0.5f);
    directionalLightCamera.projectionMatrix = sphereTraceMatrixOrthographic(-30, 30, 30, -30, -80.0f, 80.0f, 1.0f);
    directionalLightCamera.cameraSetViewMatrix();
    directionalLightCamera.cameraSetRightAndFwdVectors();
    mainCamera = Camera::cameraConstructDefault();
    mainCamera.cameraSetViewMatrix();
    mainCamera.projectionMatrix = sphereTraceMatrixPerspective(1.0f, M_PI * 0.40f, 0.1f, 1000.0f);
    pixelShaderConstantBuffer.lightColor = gVector4ColorWhite;
    pixelShaderConstantBuffer.lightDir = sphereTraceVector4ConstructWithVector3(sphereTraceVector3Negative(directionalLightCamera.cameraFwd), 1.0f);
    pScene = new SceneTest();
    pScene->pBoundCamera = &mainCamera;
    pScene->baseInit();
    pScene->init();

    // Close the command list and execute it to begin the initial GPU setup.
    ThrowIfFailed(m_commandList->Close());
    ID3D12CommandList* ppCommandLists[] = { m_commandList.Get() };
    m_commandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

    // Create synchronization objects and wait until assets have been uploaded to the GPU.
    {
        ThrowIfFailed(m_device->CreateFence(m_fenceValues[m_frameIndex], D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence)));
        m_fenceValues[m_frameIndex]++;

        // Create an event handle to use for frame synchronization.
        m_fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
        if (m_fenceEvent == nullptr)
        {
            ThrowIfFailed(HRESULT_FROM_WIN32(GetLastError()));
        }

        // Wait for the command list to execute; we are reusing the same command 
        // list in our main loop but for now, we just want to wait for setup to 
        // complete before continuing.
        WaitForGpu();
    }

    

}


// Update frame-based values.
void Renderer::OnUpdate()
{
    timer.update();
    pScene->update(timer.dt);
    pScene->updateCamera(timer.dt);

}

// Render the scene.
void Renderer::OnRender()
{
    // Record all the commands we need to render the scene into the command list.
    PopulateCommandList();

    // Execute the command list.
    ID3D12CommandList* ppCommandLists[] = { m_commandList.Get() };
    m_commandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

    // Present the frame.
    ThrowIfFailed(m_swapChain->Present(0, 0));

    MoveToNextFrame();
}

void Renderer::OnDestroy()
{
    // Ensure that the GPU is no longer referencing resources that are about to be
    // cleaned up by the destructor.
    WaitForGpu();

    CloseHandle(m_fenceEvent);
}

inline size_t Align(size_t value, size_t alignment) {
    return (value + alignment - 1) & ~(alignment - 1);
};

void Renderer::writeShadowDepthBufferToDDS()
{
    if (Input::keysDown[VK_SPACE])
    {
        m_commandList->OMSetRenderTargets(0, nullptr, FALSE, nullptr);

        // 1. Transition the depth buffer to the copy source state
        CD3DX12_RESOURCE_BARRIER transition = CD3DX12_RESOURCE_BARRIER::Transition(
            shadowDepthBuffer.Get(),
            D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, // Change to the state where it's expected to contain valid depth data
            D3D12_RESOURCE_STATE_COPY_SOURCE
        );

        m_commandList->ResourceBarrier(1, &transition);

        // 2. Create an upload heap to read back the data
        ComPtr<ID3D12Resource> readbackBuffer;
        CD3DX12_HEAP_PROPERTIES heapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_READBACK);
        CD3DX12_RESOURCE_DESC resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(Align(shadowMapWidth * shadowMapHeight * sizeof(float), D3D12_TEXTURE_DATA_PITCH_ALIGNMENT));
        m_device->CreateCommittedResource(
            &heapProps,
            D3D12_HEAP_FLAG_NONE,
            &resourceDesc, // Assuming float depth values
            D3D12_RESOURCE_STATE_COPY_DEST,
            nullptr,
            IID_PPV_ARGS(&readbackBuffer)
        );

        // 3. Define the source texture copy location
        D3D12_TEXTURE_COPY_LOCATION srcLocation = {};
        srcLocation.pResource = shadowDepthBuffer.Get();
        srcLocation.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
        srcLocation.SubresourceIndex = 0;

        // 4. Define the destination buffer copy location
        D3D12_PLACED_SUBRESOURCE_FOOTPRINT footprint;
        footprint.Offset = 0;
        footprint.Footprint.Format = DXGI_FORMAT_D32_FLOAT;
        footprint.Footprint.Width = shadowMapWidth;
        footprint.Footprint.Height = shadowMapHeight;
        footprint.Footprint.Depth = 1;
        footprint.Footprint.RowPitch = Align(shadowMapWidth * sizeof(float), D3D12_TEXTURE_DATA_PITCH_ALIGNMENT);

        D3D12_TEXTURE_COPY_LOCATION dstLocation = {};
        dstLocation.pResource = readbackBuffer.Get();
        dstLocation.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
        dstLocation.PlacedFootprint = footprint;

        // 5. Copy the depth buffer contents to the readback buffer
        CD3DX12_BOX srcBox(0, 0, 0, shadowMapWidth, shadowMapHeight, 1);
        m_commandList->CopyTextureRegion(&dstLocation, 0, 0, 0, &srcLocation, &srcBox);

        // 6. Transition the depth buffer back to its original state
        transition = CD3DX12_RESOURCE_BARRIER::Transition(
            shadowDepthBuffer.Get(),
            D3D12_RESOURCE_STATE_COPY_SOURCE,
            D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE // Change back to the original state after copying
        );

        m_commandList->ResourceBarrier(1, &transition);

        // 7. Close the command list and execute it
        m_commandList->Close();
        ID3D12CommandList* ppCommandLists[] = { m_commandList.Get() };
        m_commandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

        // 8. Wait for the GPU to finish executing the command list
        WaitForGpu();

        // 9. Read back the depth buffer contents from the readback buffer
        float* pDepthBuffer = nullptr;
        CD3DX12_RANGE readRange(0, shadowMapWidth * shadowMapHeight * sizeof(float)); // Assuming float depth values
        ThrowIfFailed(readbackBuffer->Map(0, &readRange, reinterpret_cast<void**>(&pDepthBuffer)));

        // Now pDepthBuffer contains the depth buffer data
        // Use pDepthBuffer as needed

        int width = shadowMapWidth;  // example width
        int height = shadowMapHeight; // example height

        // Create an example image: a gradient from black to white
        std::vector<unsigned char> image(width * height * 3);

        for (int y = 0; y < shadowMapHeight; ++y) {
            for (int x = 0; x < shadowMapWidth; ++x) {
                // Access depth value at (x, y)
                float depthValue = pDepthBuffer[y * (footprint.Footprint.RowPitch / sizeof(float)) + x];
                // Use depthValue as needed
                //printf("value: %f\n", depthValue);
                int offset = (y * width + x) * 3;
                image[offset] = static_cast<unsigned char>(depthValue * 255);  // R
                image[offset + 1] = 0;  // G
                image[offset + 2] = 0;  // B
            }
        }

        Texture::writeBMP("output.bmp", image.data(), width, height);

        // 10. Unmap the readback buffer
        readbackBuffer->Unmap(0, nullptr);

        exit(0);
    }

}

void Renderer::PopulateCommandList()
{
    // Command list allocators can only be reset when the associated 
    // command lists have finished execution on the GPU; apps should use 
    // fences to determine GPU execution progress.
    ThrowIfFailed(m_commandAllocators[m_frameIndex]->Reset());

    // However, when ExecuteCommandList() is called on a particular command 
    // list, that command list can then be reset at any time and must be before 
    // re-recording.
    ThrowIfFailed(m_commandList->Reset(m_commandAllocators[m_frameIndex].Get(), mPipeline.pPipelineState));

    // Set necessary state.
    
    dhp.bindDescriptorHeap(m_commandList.Get());
    cbaStack.resetAllStackIndices();

    //********************************************************SHADOW PASS************************************************************************************************
    //********************************************************SHADOW PASS************************************************************************************************
    //********************************************************SHADOW PASS************************************************************************************************
    //********************************************************SHADOW PASS************************************************************************************************
    //********************************************************SHADOW PASS************************************************************************************************
    {
        // 1. Transition the depth buffer to the depth write state
        CD3DX12_RESOURCE_BARRIER transition = CD3DX12_RESOURCE_BARRIER::Transition(
            shadowDepthBuffer.Get(),
            D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE,    // Assuming it starts in the common state
            D3D12_RESOURCE_STATE_DEPTH_WRITE
        );

        m_commandList->ResourceBarrier(1, &transition);

        // 2. Set the pipeline state and root signature
       

        // 3. Set the viewport and scissor rect
        D3D12_VIEWPORT viewport = { 0.0f, 0.0f, static_cast<float>(shadowMapWidth), static_cast<float>(shadowMapHeight), 0.0f, 1.0f };
        D3D12_RECT scissorRect = { 0, 0, static_cast<LONG>(shadowMapWidth), static_cast<LONG>(shadowMapHeight) };
        m_commandList->RSSetViewports(1, &viewport);
        m_commandList->RSSetScissorRects(1, &scissorRect);

        // 4. Set the depth stencil view
        D3D12_CPU_DESCRIPTOR_HANDLE handle = m_dsvHeap->GetCPUDescriptorHandleForHeapStart();
        handle.ptr += m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
        m_commandList->OMSetRenderTargets(0, nullptr, FALSE, &handle);

        // 5. Clear the depth buffer
        m_commandList->ClearDepthStencilView(handle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

        //Set the pixel shader vertex buffer data and bind it to Register 1
        ST_Vector3 forwardXZ = sphereTraceVector3Normalize(sphereTraceVector3Construct(mainCamera.cameraFwd.x, 0.0f, mainCamera.cameraFwd.z));
        directionalLightCamera.cameraPos = sphereTraceVector3AddAndScale(sphereTraceVector3Add(mainCamera.cameraPos, dirLightOffset), forwardXZ, 30.0f);
        Renderer::instance.pixelShaderConstantBuffer.cameraPos = sphereTraceVector4ConstructWithVector3(pScene->pBoundCamera->cameraPos, 1.0f);
        directionalLightCamera.cameraSetViewMatrix();
        lightViewProjection = sphereTraceMatrixMult(directionalLightCamera.projectionMatrix, directionalLightCamera.viewMatrix);

        // 6. Draw the scene (bind vertex/index buffers, set the root parameters, andsa issue draw calls)
        isShadowPass = true;
        pScene->pBoundCamera = &directionalLightCamera;
        m_commandList->SetPipelineState(mPipelineShadow.pPipelineState);
        m_commandList->SetGraphicsRootSignature(mRootSignitureShadow.pRootSigniture);
        pScene->draw();
        m_commandList->SetPipelineState(mPipelineShadowInstanced.pPipelineState);
        m_commandList->SetGraphicsRootSignature(mRootSignitureShadow.pRootSigniture);
        Renderer::instance.drawAddedPrimitiveInstances();

        // 7. Transition the depth buffer to a readable state (optional)
        transition = CD3DX12_RESOURCE_BARRIER::Transition(
            shadowDepthBuffer.Get(),
            D3D12_RESOURCE_STATE_DEPTH_WRITE,
            D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE
        );
        m_commandList->ResourceBarrier(1, &transition);

    }
    //writeShadowDepthBufferToDDS();
    //********************************************************RENDER PASS************************************************************************************************
    //********************************************************RENDER PASS************************************************************************************************
    //********************************************************RENDER PASS************************************************************************************************
    //********************************************************RENDER PASS************************************************************************************************
    //********************************************************RENDER PASS************************************************************************************************
    // Indicate that the back buffer will be used as a render target.
    {
        CD3DX12_RESOURCE_BARRIER resourceBarrier = CD3DX12_RESOURCE_BARRIER::Transition(m_renderTargets[m_frameIndex].Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
        m_commandList->ResourceBarrier(1, &resourceBarrier);
        resourceBarrier = CD3DX12_RESOURCE_BARRIER::Transition(m_depthStencil.Get(), D3D12_RESOURCE_STATE_DEPTH_READ, D3D12_RESOURCE_STATE_DEPTH_WRITE);
        m_commandList->ResourceBarrier(1, &resourceBarrier);

        m_commandList->RSSetViewports(1, &m_viewport);
        m_commandList->RSSetScissorRects(1, &m_scissorRect);

        CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_rtvHeap->GetCPUDescriptorHandleForHeapStart(), m_frameIndex, m_rtvDescriptorSize);
        CD3DX12_CPU_DESCRIPTOR_HANDLE dsvHandle(m_dsvHeap->GetCPUDescriptorHandleForHeapStart());

        m_commandList->OMSetRenderTargets(1, &rtvHandle, FALSE, &dsvHandle);

        // Record commands.
        const float clearColor[] = { 0.0f, 0.0f, 0.0f, 1.0f };
        m_commandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
        m_commandList->ClearDepthStencilView(m_dsvHeap->GetCPUDescriptorHandleForHeapStart(), D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);



        //draw scene
        isShadowPass = false;
        pScene->pBoundCamera = &mainCamera;
        pScene->draw();
        drawAddedPrimitiveInstances();
        drawAddedWireFrameInstances();

        // Indicate that the back buffer will now be used to present.
        resourceBarrier = CD3DX12_RESOURCE_BARRIER::Transition(m_renderTargets[m_frameIndex].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
        m_commandList->ResourceBarrier(1, &resourceBarrier);
        resourceBarrier = CD3DX12_RESOURCE_BARRIER::Transition(m_depthStencil.Get(), D3D12_RESOURCE_STATE_DEPTH_WRITE, D3D12_RESOURCE_STATE_DEPTH_READ);
        m_commandList->ResourceBarrier(1, &resourceBarrier);
    }


    ThrowIfFailed(m_commandList->Close());
}

// Wait for pending GPU work to complete.
void Renderer::WaitForGpu()
{
    // Schedule a Signal command in the queue.
    ThrowIfFailed(m_commandQueue->Signal(m_fence.Get(), m_fenceValues[m_frameIndex]));

    // Wait until the fence has been processed.
    ThrowIfFailed(m_fence->SetEventOnCompletion(m_fenceValues[m_frameIndex], m_fenceEvent));
    WaitForSingleObjectEx(m_fenceEvent, INFINITE, FALSE);

    // Increment the fence value for the current frame.
    m_fenceValues[m_frameIndex]++;
}

// Prepare to render the next frame.
void Renderer::MoveToNextFrame()
{
    // Schedule a Signal command in the queue.
    const UINT64 currentFenceValue = m_fenceValues[m_frameIndex];
    ThrowIfFailed(m_commandQueue->Signal(m_fence.Get(), currentFenceValue));

    // Update the frame index.
    m_frameIndex = m_swapChain->GetCurrentBackBufferIndex();

    // If the next frame is not ready to be rendered yet, wait until it is ready.
    if (m_fence->GetCompletedValue() < m_fenceValues[m_frameIndex])
    {
        ThrowIfFailed(m_fence->SetEventOnCompletion(m_fenceValues[m_frameIndex], m_fenceEvent));
        WaitForSingleObjectEx(m_fenceEvent, INFINITE, FALSE);
    }

    // Set the fence value for the next frame.
    m_fenceValues[m_frameIndex] = currentFenceValue + 1;
}

void Renderer::drawPrimitive(ST_Vector3 position, ST_Quaternion rotation, ST_Vector3 scale, Texture& texture, PrimitiveType type)
{

    m_constantBufferData.model = sphereTraceMatrixMult(sphereTraceMatrixTranslation(position),
        sphereTraceMatrixMult(sphereTraceMatrixFromQuaternion(rotation), sphereTraceMatrixScale(scale)));
    m_commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    m_constantBufferData.mvp = sphereTraceMatrixMult(pScene->pBoundCamera->projectionMatrix, sphereTraceMatrixMult(pScene->pBoundCamera->viewMatrix, m_constantBufferData.model));
    m_constantBufferData.colorMix = 0.0f;
    m_constantBufferData.lightViewProjection = lightViewProjection;
    if (isShadowPass)
    {
        cbaStack.updateBindAndIncrementCurrentAccessor(2, &m_constantBufferData.mvp, m_commandList.Get(), 0);
    }
    else
    {
        m_commandList->SetPipelineState(mPipeline.pPipelineState);
        m_commandList->SetGraphicsRootSignature(mRootSigniture.pRootSigniture);
        texture.bind(m_commandList.Get(), 2);
        m_commandList->SetGraphicsRootDescriptorTable(3, shadowSrvGpuHandle);
        cbaStack.updateBindAndIncrementCurrentAccessor(0, &m_constantBufferData.mvp, m_commandList.Get(), 0);
        pixelShaderConstantBufferAccessor.updateConstantBufferData((void*)&pixelShaderConstantBuffer);
        pixelShaderConstantBufferAccessor.bind(m_commandList.Get(), 1);
    }
    switch (type)
    {
    case PRIMITIVE_PLANE:
        mPlaneVB.draw(m_commandList.Get());
        break;
    case PRIMITIVE_BOX:
        mCubeVB.draw(m_commandList.Get());
        break;
    case PRIMITIVE_SPHERE:
        mSphereVB.draw(m_commandList.Get());
        break;
    case PRIMITIVE_CYLINDER:
        mCylinderVB.draw(m_commandList.Get());
    }
    
}

void Renderer::drawPrimitive(ST_Vector3 position, ST_Quaternion rotation, ST_Vector3 scale, ST_Vector4 color, PrimitiveType type)
{

    m_constantBufferData.model = sphereTraceMatrixMult(sphereTraceMatrixTranslation(position),
        sphereTraceMatrixMult(sphereTraceMatrixFromQuaternion(rotation), sphereTraceMatrixScale(scale)));
    m_constantBufferData.mvp = sphereTraceMatrixMult(pScene->pBoundCamera->projectionMatrix, sphereTraceMatrixMult(pScene->pBoundCamera->viewMatrix, m_constantBufferData.model));
    m_constantBufferData.color = color;
    m_constantBufferData.colorMix = 1.0f;
    m_constantBufferData.lightViewProjection = lightViewProjection;
    m_commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    if (isShadowPass)
    {
        cbaStack.updateBindAndIncrementCurrentAccessor(2, &m_constantBufferData.mvp, m_commandList.Get(), 0);
    }
    else
    {
        m_commandList->SetPipelineState(mPipeline.pPipelineState);
        m_commandList->SetGraphicsRootSignature(mRootSigniture.pRootSigniture);
        m_commandList->SetGraphicsRootDescriptorTable(3, shadowSrvGpuHandle);
        cbaStack.updateBindAndIncrementCurrentAccessor(0, &m_constantBufferData.mvp, m_commandList.Get(), 0);
        pixelShaderConstantBufferAccessor.updateConstantBufferData((void*)&pixelShaderConstantBuffer);
        pixelShaderConstantBufferAccessor.bind(m_commandList.Get(), 1);
    }
    switch (type)
    {
    case PRIMITIVE_PLANE:
        mPlaneVB.draw(m_commandList.Get());
        break;
    case PRIMITIVE_BOX:
        mCubeVB.draw(m_commandList.Get());
        break;
    case PRIMITIVE_SPHERE:
        mSphereVB.draw(m_commandList.Get());
        break;
    case PRIMITIVE_CYLINDER:
        mCylinderVB.draw(m_commandList.Get());
    }
}

void Renderer::drawPrimitive(ST_Vector3 position, ST_Quaternion rotation, ST_Vector3 scale, ST_Vector4 color, Texture& texture, float colorMix, PrimitiveType type)
{
    m_constantBufferData.model = sphereTraceMatrixMult(sphereTraceMatrixTranslation(position),
        sphereTraceMatrixMult(sphereTraceMatrixFromQuaternion(rotation), sphereTraceMatrixScale(scale)));
    m_commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    m_constantBufferData.mvp = sphereTraceMatrixMult(pScene->pBoundCamera->projectionMatrix, sphereTraceMatrixMult(pScene->pBoundCamera->viewMatrix, m_constantBufferData.model));
    m_constantBufferData.colorMix = colorMix;
    m_constantBufferData.color = color;
    m_constantBufferData.lightViewProjection = lightViewProjection;
    if (isShadowPass)
    {
        cbaStack.updateBindAndIncrementCurrentAccessor(2, &m_constantBufferData.mvp, m_commandList.Get(), 0);
    }
    else
    {
        m_commandList->SetPipelineState(mPipeline.pPipelineState);
        m_commandList->SetGraphicsRootSignature(mRootSigniture.pRootSigniture);
        texture.bind(m_commandList.Get(), 2);
        m_commandList->SetGraphicsRootDescriptorTable(3, shadowSrvGpuHandle);
        cbaStack.updateBindAndIncrementCurrentAccessor(0, &m_constantBufferData.mvp, m_commandList.Get(), 0);
        pixelShaderConstantBufferAccessor.updateConstantBufferData((void*)&pixelShaderConstantBuffer);
        pixelShaderConstantBufferAccessor.bind(m_commandList.Get(), 1);
    }
    switch (type)
    {
    case PRIMITIVE_PLANE:
        mPlaneVB.draw(m_commandList.Get());
        break;
    case PRIMITIVE_BOX:
        mCubeVB.draw(m_commandList.Get());
        break;
    case PRIMITIVE_SPHERE:
        mSphereVB.draw(m_commandList.Get());
        break;
    case PRIMITIVE_CYLINDER:
        mCylinderVB.draw(m_commandList.Get());
    }
}

void Renderer::drawVertexBuffer(ST_Vector3 position, ST_Quaternion rotation, ST_Vector3 scale, ST_Vector4 color, Texture& texture, float colorMix, VertexBuffer& vertexBuffer)
{
    m_constantBufferData.model = sphereTraceMatrixMult(sphereTraceMatrixTranslation(position),
        sphereTraceMatrixMult(sphereTraceMatrixFromQuaternion(rotation), sphereTraceMatrixScale(scale)));
    m_commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    m_constantBufferData.mvp = sphereTraceMatrixMult(pScene->pBoundCamera->projectionMatrix, sphereTraceMatrixMult(pScene->pBoundCamera->viewMatrix, m_constantBufferData.model));
    m_constantBufferData.colorMix = colorMix;
    m_constantBufferData.color = color;
    m_constantBufferData.lightViewProjection = lightViewProjection;
    if (isShadowPass)
    {
        cbaStack.updateBindAndIncrementCurrentAccessor(2, &m_constantBufferData.mvp, m_commandList.Get(), 0);
    }
    else
    {
        m_commandList->SetPipelineState(mPipeline.pPipelineState);
        m_commandList->SetGraphicsRootSignature(mRootSigniture.pRootSigniture);
        texture.bind(m_commandList.Get(), 2);
        m_commandList->SetGraphicsRootDescriptorTable(3, shadowSrvGpuHandle);
        cbaStack.updateBindAndIncrementCurrentAccessor(0, &m_constantBufferData.mvp, m_commandList.Get(), 0);
        pixelShaderConstantBufferAccessor.updateConstantBufferData((void*)&pixelShaderConstantBuffer);
        pixelShaderConstantBufferAccessor.bind(m_commandList.Get(), 1);
    }
    vertexBuffer.draw(m_commandList.Get());
}

void Renderer::drawModel(ST_Vector3 position, ST_Quaternion rotation, ST_Vector3 scale, ST_Vector4 color, float colorMix, Model& model)
{
    m_constantBufferData.model = sphereTraceMatrixMult(sphereTraceMatrixTranslation(position),
        sphereTraceMatrixMult(sphereTraceMatrixFromQuaternion(rotation), sphereTraceMatrixScale(scale)));
    m_commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    m_constantBufferData.mvp = sphereTraceMatrixMult(pScene->pBoundCamera->projectionMatrix, sphereTraceMatrixMult(pScene->pBoundCamera->viewMatrix, m_constantBufferData.model));
    m_constantBufferData.colorMix = colorMix;
    m_constantBufferData.color = color;
    m_constantBufferData.lightViewProjection = lightViewProjection;
    if (isShadowPass)
    {
        cbaStack.updateBindAndIncrementCurrentAccessor(2, &m_constantBufferData.mvp, m_commandList.Get(), 0);
    }
    else
    {
        m_commandList->SetPipelineState(mPipeline.pPipelineState);
        m_commandList->SetGraphicsRootSignature(mRootSigniture.pRootSigniture);
        m_commandList->SetGraphicsRootDescriptorTable(3, shadowSrvGpuHandle);
        cbaStack.updateBindAndIncrementCurrentAccessor(0, &m_constantBufferData.mvp, m_commandList.Get(), 0);
        pixelShaderConstantBufferAccessor.updateConstantBufferData((void*)&pixelShaderConstantBuffer);
        pixelShaderConstantBufferAccessor.bind(m_commandList.Get(), 1);
    }
    for (Model::ModelComponent& component: model.components)
    {
        if (!isShadowPass && component.materia.hasTexture)
        {
            component.materia.ambientTexture.bind(m_commandList.Get(), 2);
        }
        component.vb.draw(m_commandList.Get());
    }
}



void Renderer::drawWireFrame(ST_Vector3 position, ST_Quaternion rotation, ST_Vector3 scale, ST_Vector4 color, PrimitiveType type)
{
    if (isShadowPass)
		return;
	ST_Matrix4 model = sphereTraceMatrixMult(sphereTraceMatrixTranslation(position),
		sphereTraceMatrixMult(sphereTraceMatrixFromQuaternion(rotation), sphereTraceMatrixScale(scale)));
	m_commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);
	m_constantBufferData.mvp = sphereTraceMatrixMult(pScene->pBoundCamera->projectionMatrix, sphereTraceMatrixMult(pScene->pBoundCamera->viewMatrix, model));
	m_constantBufferData.color = color;
	m_commandList->SetPipelineState(mPipelineWireFrame.pPipelineState);
	m_commandList->SetGraphicsRootSignature(mRootSignitureWireFrame.pRootSigniture);
	cbaStack.updateBindAndIncrementCurrentAccessor(0, (void*)&m_constantBufferData.mvp, m_commandList.Get(), 0);

	switch (type)
    {
    case PRIMITIVE_PLANE:
        mGridVB.draw(m_commandList.Get());
        break;
    case PRIMITIVE_BOX:
        mCubeWireFrameVB.draw(m_commandList.Get());
        break;
    case PRIMITIVE_SPHERE:
        mSphereWireFrameVB.draw(m_commandList.Get());
        break;
    }
}

void Renderer::drawLine(const ST_Vector3& from, const ST_Vector3& to, const ST_Vector4& color)
{
    ST_Vector3 dir = sphereTraceVector3Subtract(to, from);
    float dist = sphereTraceVector3Length(dir);
    dir = sphereTraceVector3Normalize(dir);
    ST_Vector3 dirXZ = { dir.x, 0, dir.z };
    dirXZ = sphereTraceVector3Normalize(dirXZ);
    float theta = -asinf(dirXZ.z);
    theta = dirXZ.x <= 0.0f ? (3.14159f - theta) : theta;
    float psi = atanf(dir.y / sqrtf(dir.x * dir.x + dir.z * dir.z));
    if (dir.x == 0.0f && dir.z == 0.0f || sphereTraceVector3NanAny(dirXZ))
    {
        if (dir.y > 0.0f)
            psi = 3.14159f * 0.5f;
        else
            psi = -3.141595f * 0.5f;
        theta = 0.0f;
    }
    ST_Matrix4 modelMatrix = sphereTraceMatrixIdentity();
    modelMatrix = sphereTraceMatrixTranslation(from);
    modelMatrix = sphereTraceMatrixMult(modelMatrix, sphereTraceMatrixRotateY(theta));
    modelMatrix = sphereTraceMatrixMult(modelMatrix, sphereTraceMatrixRotateZ(psi));
    modelMatrix = sphereTraceMatrixMult(modelMatrix, sphereTraceMatrixScale({ dist, 1, 1 }));
    ST_Matrix4 modelView = sphereTraceMatrixMult(pScene->pBoundCamera->viewMatrix, modelMatrix);
    ST_Matrix4 modelViewProjection = sphereTraceMatrixMult(pScene->pBoundCamera->projectionMatrix, modelView);

    m_commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);
    m_constantBufferData.mvp = modelViewProjection;
    if (isShadowPass)
    {
        return;
        cbaStack.updateBindAndIncrementCurrentAccessor(2, &m_constantBufferData.mvp, m_commandList.Get(), 0);
    }
    else
    {
        m_commandList->SetPipelineState(mPipelineWireFrame.pPipelineState);
        m_commandList->SetGraphicsRootSignature(mRootSignitureWireFrame.pRootSigniture);
        cbaStack.updateBindAndIncrementCurrentAccessor(0, &m_constantBufferData.mvp, m_commandList.Get(), 0);
        cbaStack.updateBindAndIncrementCurrentAccessor(1, (void*)&color, m_commandList.Get(), 1);
    }
    mLineVB.draw(m_commandList.Get());
}


void Renderer::addPrimitiveInstance(ST_Vector3 position, ST_Quaternion rotation, ST_Vector3 scale, ST_Vector4 color, PrimitiveType type)
{

    if (isShadowPass)
    {
        UINT stackInd = perPrimitiveInstanceBufferCountsShadows[type] / 400;
        UINT bufInd = perPrimitiveInstanceBufferCountsShadows[type] % 400;
        ST_Matrix4 model = sphereTraceMatrixMult(sphereTraceMatrixTranslation(position),
            sphereTraceMatrixMult(sphereTraceMatrixFromQuaternion(rotation), sphereTraceMatrixScale(scale)));
        perPrimitiveInstanceBufferShadows[type][stackInd].mvp[bufInd] = sphereTraceMatrixMult(pScene->pBoundCamera->projectionMatrix, sphereTraceMatrixMult(pScene->pBoundCamera->viewMatrix,
            model));
        perPrimitiveInstanceBufferCountsShadows[type]++;
        if (perPrimitiveInstanceBufferCountsShadows[type] >= perPrimitiveInstanceBufferCapacitiesShadows[type])
        {
            ConstantBufferAccessor cba;
            perPrimitiveInstanceCBAAccessorsShadows[type].push_back(cba);
            UINT newStackCap = perPrimitiveInstanceCBAAccessorsShadows[type].size() - 1;
            perPrimitiveInstanceCBAAccessorsShadows[type][newStackCap].init(m_device.Get(), dhp, &perPrimitiveInstanceBuffers[0][0], sizeof(VertexShaderInstancedConstantBufferShadows));

            perPrimitiveInstanceBufferShadows[type].push_back(shadowInstanceBuf);
            perPrimitiveInstanceBufferCapacitiesShadows[type] = (newStackCap + 1) * 400;
        }
    }
    else
    {
        UINT stackInd = perPrimitiveInstanceBufferCounts[type] / 400;
        UINT bufInd = perPrimitiveInstanceBufferCounts[type] % 400;
        perPrimitiveInstanceBuffers[type][stackInd].model[bufInd] = sphereTraceMatrixMult(sphereTraceMatrixTranslation(position),
            sphereTraceMatrixMult(sphereTraceMatrixFromQuaternion(rotation), sphereTraceMatrixScale(scale)));
        perPrimitiveInstanceBuffers[type][stackInd].colors[bufInd] = color;

        perPrimitiveInstanceBufferCounts[type]++;
        if (perPrimitiveInstanceBufferCounts[type] >= perPrimitiveInstanceBufferCapacities[type])
        {
            ConstantBufferAccessor cba;
            perPrimitiveInstanceCBAAccessors[type].push_back(cba);
            UINT newStackCap = perPrimitiveInstanceCBAAccessors[type].size() - 1;
            perPrimitiveInstanceCBAAccessors[type][newStackCap].init(m_device.Get(), dhp, &perPrimitiveInstanceBuffers[0][0], sizeof(WireFrameInstancedConstantBuffer));

            perPrimitiveInstanceBuffers[type].push_back(instanceBuf);
            perPrimitiveInstanceBufferCapacities[type] = (newStackCap + 1) * 400;
        }
    }
}


void Renderer::addWireFrameInstance(ST_Vector3 position, ST_Quaternion rotation, ST_Vector3 scale, ST_Vector4 color, PrimitiveType type)
{
    if (isShadowPass)
        return;
    UINT stackInd = perWireFramePrimitiveInstanceBufferCounts[type] / 400;
    UINT bufInd = perWireFramePrimitiveInstanceBufferCounts[type] % 400;
    perWireFramePrimitiveInstanceBuffers[type][stackInd].model[bufInd] = sphereTraceMatrixMult(sphereTraceMatrixTranslation(position),
        sphereTraceMatrixMult(sphereTraceMatrixFromQuaternion(rotation), sphereTraceMatrixScale(scale)));
    perWireFramePrimitiveInstanceBuffers[type][stackInd].colors[bufInd] = color;

	perWireFramePrimitiveInstanceBufferCounts[type]++;
	if (perWireFramePrimitiveInstanceBufferCounts[type] >= perWireFramePrimitiveInstanceBufferCapacities[type])
	{
        ConstantBufferAccessor cba;
		perWireFramePrimitiveInstanceCBAAccessors[type].push_back(cba);
        UINT newStackCap = perWireFramePrimitiveInstanceCBAAccessors[type].size() - 1;
		perWireFramePrimitiveInstanceCBAAccessors[type][newStackCap].init(m_device.Get(), dhp, &perPrimitiveInstanceBuffers[0][0], sizeof(WireFrameInstancedConstantBuffer));
        
		perWireFramePrimitiveInstanceBuffers[type].push_back(instanceBuf);
        perWireFramePrimitiveInstanceBufferCapacities[type] = (newStackCap + 1) * 400;
	}
}
void Renderer::drawAddedWireFrameInstances()
{
    if (isShadowPass)
        return;
    for (int i = 0; i < 3; i++)
    {
        PrimitiveType type = (PrimitiveType)i;
        if (perWireFramePrimitiveInstanceBufferCounts[type] == 0)
            continue;

        m_commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);
        m_commandList->SetPipelineState(mPipelineWireFrame.pPipelineState);
        m_commandList->SetGraphicsRootSignature(mRootSignitureWireFrame.pRootSigniture);

        UINT numStacks = (perWireFramePrimitiveInstanceBufferCounts[type] / 400)+1;
        for (int j = 0; j < numStacks; j++)
        {
            perWireFramePrimitiveInstanceBuffers[type][j].viewProjection = sphereTraceMatrixMult(pScene->pBoundCamera->projectionMatrix, pScene->pBoundCamera->viewMatrix);
            perWireFramePrimitiveInstanceCBAAccessors[type][j].updateConstantBufferData(&perWireFramePrimitiveInstanceBuffers[type][j]);
            perWireFramePrimitiveInstanceCBAAccessors[type][j].bind(m_commandList.Get(), 0);
            UINT numInstance = j < numStacks - 1 ? 400 : perWireFramePrimitiveInstanceBufferCounts[type] % 400;
            switch (type)
            {
            case PRIMITIVE_PLANE:
                mGridVB.drawInstanced(m_commandList.Get(), numInstance);
                break;
            case PRIMITIVE_BOX:
                mCubeWireFrameVB.drawInstanced(m_commandList.Get(), numInstance);
                break;
            case PRIMITIVE_SPHERE:
                mSphereWireFrameVB.drawInstanced(m_commandList.Get(), numInstance);
                break;
            }
        }
        perWireFramePrimitiveInstanceBufferCounts[type] = 0;
    }
}

void Renderer::drawAddedPrimitiveInstances()
{
    for (int i = 0; i < 4; i++)
    {
        PrimitiveType type = (PrimitiveType)i;

        if (isShadowPass)
        {
            if (perPrimitiveInstanceBufferCountsShadows[type] == 0)
                continue;
            m_commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

            UINT numStacks = (perPrimitiveInstanceBufferCountsShadows[type] / 400) + 1;
            for (int j = 0; j < numStacks; j++)
            {
                perPrimitiveInstanceCBAAccessorsShadows[type][j].updateConstantBufferData(&perPrimitiveInstanceBufferShadows[type][j]);
                perPrimitiveInstanceCBAAccessorsShadows[type][j].bind(m_commandList.Get(), 0);

                switch (type)
                {
                case PRIMITIVE_PLANE:
                    mPlaneVB.drawInstanced(m_commandList.Get(), perPrimitiveInstanceBufferCountsShadows[type]);
                    break;
                case PRIMITIVE_BOX:
                    mCubeVB.drawInstanced(m_commandList.Get(), perPrimitiveInstanceBufferCountsShadows[type]);
                    break;
                case PRIMITIVE_SPHERE:
                    mSphereVB.drawInstanced(m_commandList.Get(), perPrimitiveInstanceBufferCountsShadows[type]);
                    break;
                case PRIMITIVE_CYLINDER:
                    mCylinderVB.drawInstanced(m_commandList.Get(), perPrimitiveInstanceBufferCountsShadows[type]);
                }
            }
            perPrimitiveInstanceBufferCountsShadows[type] = 0;
        }
        else
        {
            if (perPrimitiveInstanceBufferCounts[type] == 0)
                continue;
            m_commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
            m_commandList->SetPipelineState(mPipelineInstanced.pPipelineState);
            m_commandList->SetGraphicsRootSignature(mRootSignitureInstanced.pRootSigniture);

            UINT numStacks = (perPrimitiveInstanceBufferCounts[type] / 400) + 1;
            for (int j = 0; j < numStacks; j++)
            {
                perPrimitiveInstanceBuffers[type][j].lightViewProj = lightViewProjection;
                perPrimitiveInstanceBuffers[type][j].viewProjection = sphereTraceMatrixMult(pScene->pBoundCamera->projectionMatrix, pScene->pBoundCamera->viewMatrix);
                perPrimitiveInstanceCBAAccessors[type][j].updateConstantBufferData(&perPrimitiveInstanceBuffers[type][j]);
                perPrimitiveInstanceCBAAccessors[type][j].bind(m_commandList.Get(), 0);
                pixelShaderConstantBufferAccessor.updateConstantBufferData((void*)&pixelShaderConstantBuffer);
                pixelShaderConstantBufferAccessor.bind(m_commandList.Get(), 1);
                m_commandList->SetGraphicsRootDescriptorTable(2, shadowSrvGpuHandle);

                UINT numInstance = j < numStacks - 1 ? 400 : perPrimitiveInstanceBufferCounts[type] % 400;
                switch (type)
                {
                case PRIMITIVE_PLANE:
                    mPlaneVB.drawInstanced(m_commandList.Get(), numInstance);
                    break;
                case PRIMITIVE_BOX:
                    mCubeVB.drawInstanced(m_commandList.Get(), numInstance);
                    break;
                case PRIMITIVE_SPHERE:
                    mSphereVB.drawInstanced(m_commandList.Get(), numInstance);
                    break;
                case PRIMITIVE_CYLINDER:
                    mCylinderVB.drawInstanced(m_commandList.Get(), numInstance);
                }
            }
            perPrimitiveInstanceBufferCounts[type] = 0;
        }
    }
}



