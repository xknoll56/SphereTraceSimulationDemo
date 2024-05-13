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
#include "Input.h"

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
        dsvHeapDesc.NumDescriptors = 1; // Number of descriptors (usually one for depth buffer)
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

         CD3DX12_DESCRIPTOR_RANGE1 ranges[2];
         CD3DX12_ROOT_PARAMETER1 rootParameters[2];
         ranges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC);
         ranges[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC);
         rootParameters[0].InitAsDescriptorTable(1, &ranges[0], D3D12_SHADER_VISIBILITY_VERTEX);
         rootParameters[1].InitAsDescriptorTable(1, &ranges[1], D3D12_SHADER_VISIBILITY_PIXEL);

        D3D12_STATIC_SAMPLER_DESC sampler = {};
        sampler.Filter = D3D12_FILTER_MIN_MAG_MIP_POINT;
        sampler.AddressU = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
        sampler.AddressV = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
        sampler.AddressW = D3D12_TEXTURE_ADDRESS_MODE_BORDER;
        sampler.MipLODBias = 0;
        sampler.MaxAnisotropy = 0;
        sampler.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
        sampler.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
        sampler.MinLOD = 0.0f;
        sampler.MaxLOD = D3D12_FLOAT32_MAX;
        sampler.ShaderRegister = 0;
        sampler.RegisterSpace = 0;
        sampler.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;


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

        mRootSigniture.init(m_device.Get(), rootParameters, 2, rootSignatureFlags, sampler);
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

        CD3DX12_DESCRIPTOR_RANGE1 ranges[1];
        CD3DX12_ROOT_PARAMETER1 rootParameters[1];

        ranges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0, 0, D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC);
        rootParameters[0].InitAsDescriptorTable(1, &ranges[0], D3D12_SHADER_VISIBILITY_VERTEX);

        // Allow input layout and deny uneccessary access to certain pipeline stages.
        D3D12_ROOT_SIGNATURE_FLAGS rootSignatureFlags =
            D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
            D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
            D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
            D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS |
            D3D12_ROOT_SIGNATURE_FLAG_DENY_PIXEL_SHADER_ROOT_ACCESS;

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

    // Create the command list.
    ThrowIfFailed(m_device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_commandAllocators[m_frameIndex].Get(), mPipeline.pPipelineState, IID_PPV_ARGS(&m_commandList)));



    mCubeVB = VertexBuffer::createCube(m_device.Get());
    mCubeWireFrameVB = VertexBuffer::createCubeWireFrame(m_device.Get());
    mPlaneVB = VertexBuffer::createPlane(m_device.Get());
    mSphereVB = VertexBuffer::createSphere(m_device.Get());

    //create constant buffer
    //for(int i = 0; i<400; i++)
    //    mConstantBufferAccessors[i].init(m_device.Get(), dhp, &m_constantBufferData, sizeof(SceneConstantBuffer));
    //mConstantBufferAccessors[400].init(m_device.Get(), dhp, &m_constantBufferData, sizeof(SceneConstantBuffer));
    cbaStack = ConstantBufferAccessorStack(3);
    UINT cbastackhandle = cbaStack.createStack(m_device.Get(), dhp, sizeof(SceneConstantBuffer), 500);


    // Note: ComPtr's are CPU objects but this resource needs to stay in scope until
      // the command list that references it has finished executing on the GPU.
      // We will flush the GPU at the end of this method to ensure the resource is not
      // prematurely destroyed.

    ComPtr<ID3D12Resource> textureUploadHeap;

    // Copy data to the intermediate upload heap and then schedule a copy 
// from the upload heap to the Texture2D.
    unsigned char texBytes[] = {
        255, 255, 255, 255,
        0,  0, 0, 255,
        0, 0, 0, 255,
        255, 255, 255, 255
    };

    texture.init(m_device.Get(), m_commandList.Get(),textureUploadHeap.Get(), dhp,
        2, 2, texBytes);

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
    }

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

    sphereTraceAllocatorInitialize();

    //ST_Matrix4 model = sphereTraceMatrixScale(ST_VECTOR3(50, 50, 50));
    //ST_Matrix4 view = sphereTraceMatrixLookAt(ST_VECTOR3(30, 30, 30), gVector3Zero, gVector3Up);
    //ST_Matrix4 projection = sphereTraceMatrixPerspective(1.0f, M_PI * 0.40f, 0.1f, 1000.0f);
    //m_constantBufferData.mvp = sphereTraceMatrixMult(projection, sphereTraceMatrixMult(view, model));
    //mConstantBufferAccessors[400].updateConstantBufferData(&m_constantBufferData.mvp);
    camera = Camera::cameraConstructDefault();
    camera.cameraSetViewMatrix();
}


// Update frame-based values.
void Renderer::OnUpdate()
{
    timer.update();

    if (Input::keys[KEY_W])
    {
        camera.cameraLerp = sphereTraceVector3Add(sphereTraceVector3Scale(camera.cameraFwd, camera.cameraMovementSpeed * timer.dt), camera.cameraLerp);
    }
    if (Input::keys[KEY_A])
    {
        camera.cameraLerp = sphereTraceVector3Add(sphereTraceVector3Scale(camera.cameraRight, -camera.cameraMovementSpeed * timer.dt), camera.cameraLerp);
    }
    if (Input::keys[KEY_S])
    {
        camera.cameraLerp = sphereTraceVector3Add(sphereTraceVector3Scale(camera.cameraFwd, -camera.cameraMovementSpeed * timer.dt), camera.cameraLerp);
    }
    if (Input::keys[KEY_D])
    {
        camera.cameraLerp = sphereTraceVector3Add(sphereTraceVector3Scale(camera.cameraRight, camera.cameraMovementSpeed * timer.dt), camera.cameraLerp);
    }

    if (Input::mouse[MOUSE_RIGHT])
    {
        camera.cameraLerpYaw += Input::gDeltaMousePosition.x * camera.cameraTurningSpeed * timer.dt;
        camera.cameraLerpPitch += Input::gDeltaMousePosition.y * camera.cameraTurningSpeed * timer.dt;
    }
    camera.cameraPos = sphereTraceVector3Lerp(camera.cameraPos, camera.cameraLerp, timer.dt * camera.lerpSpeed);
    camera.cameraYaw = sphereTraceLerp(camera.cameraYaw, camera.cameraLerpYaw, timer.dt * camera.lerpSpeed);
    camera.cameraPitch = sphereTraceLerp(camera.cameraPitch, camera.cameraLerpPitch, timer.dt * camera.lerpSpeed);
    camera.cameraSetViewMatrix();
    camera.cameraSetRightAndFwdVectors();
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
    cbaStack.resetStackIndex(0);




    // Indicate that the back buffer will be used as a render target.
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
    const float clearColor[] = { 0.0f, 0.2f, 0.4f, 1.0f };
    m_commandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
    m_commandList->ClearDepthStencilView(m_dsvHeap->GetCPUDescriptorHandleForHeapStart(), D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);


    

    
    //mConstantBufferAccessors[0].bind(m_commandList.Get());
    //ppHeaps[0] =  m_cbvHeap.Get();
    //m_commandList->SetDescriptorHeaps(1, ppHeaps);

    m_constantBufferData.color = gVector4ColorGreen;
    for (int i = 0; i < 20; i++)
    {
        for (int j = 0; j < 20; j++)
        {
            int index = i * 20 + j;
            ST_Matrix4 model = sphereTraceMatrixMult(sphereTraceMatrixRotateY(timer.currentTimeInSeconds), sphereTraceMatrixTranslation(ST_VECTOR3(i, 0, j)));
            //ST_Matrix4 view = sphereTraceMatrixLookAt(ST_VECTOR3(30, 30, 30), gVector3Zero, gVector3Up);
            ST_Matrix4 view = camera.viewMatrix;
            ST_Matrix4 projection = sphereTraceMatrixPerspective(1.0f, M_PI * 0.40f, 0.1f, 1000.0f);
            m_commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
            m_commandList->SetPipelineState(mPipeline.pPipelineState);
            m_commandList->SetGraphicsRootSignature(mRootSigniture.pRootSigniture);
            texture.bind(m_commandList.Get(), 1);
            m_constantBufferData.mvp = sphereTraceMatrixMult(projection, sphereTraceMatrixMult(view, model));
            cbaStack.updateAndBindtCurrentAccessor(0, &m_constantBufferData.mvp, m_commandList.Get(), 0);
            mSphereVB.draw(m_commandList.Get());


			m_commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);
			m_commandList->SetPipelineState(mPipelineWireFrame.pPipelineState);
			m_commandList->SetGraphicsRootSignature(mRootSignitureWireFrame.pRootSigniture);
			cbaStack.bindCurrentAccessor(0, m_commandList.Get(), 0);
			mCubeWireFrameVB.draw(m_commandList.Get());
			cbaStack.incrementStackIndex(0);
        }
    }
    //mConstantBufferAccessors[400].bind(m_commandList.Get());
    //mPlaneVB.draw(m_commandList.Get());
    // Indicate that the back buffer will now be used to present.
    resourceBarrier = CD3DX12_RESOURCE_BARRIER::Transition(m_renderTargets[m_frameIndex].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
    m_commandList->ResourceBarrier(1, &resourceBarrier);
    resourceBarrier = CD3DX12_RESOURCE_BARRIER::Transition(m_depthStencil.Get(), D3D12_RESOURCE_STATE_DEPTH_WRITE, D3D12_RESOURCE_STATE_DEPTH_READ);
    m_commandList->ResourceBarrier(1, &resourceBarrier);

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
