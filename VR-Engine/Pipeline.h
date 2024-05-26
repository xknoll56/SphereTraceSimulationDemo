#pragma once

#include "stdafx.h"

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


    void init(ID3D12Device* pDevice, CD3DX12_ROOT_PARAMETER1* rootParameters, UINT numRootParameters, D3D12_ROOT_SIGNATURE_FLAGS flags, D3D12_STATIC_SAMPLER_DESC* samplerDescs, UINT numSamplers)
    {

        if (FAILED(pDevice->CheckFeatureSupport(D3D12_FEATURE_ROOT_SIGNATURE, &featureData, sizeof(featureData))))
        {
            featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_0;
        }

        CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc;
        rootSignatureDesc.Init_1_1(numRootParameters, rootParameters, numSamplers, samplerDescs, flags);

        ComPtr<ID3DBlob> signature;
        ComPtr<ID3DBlob> error;
        HRESULT hr = D3DX12SerializeVersionedRootSignature(&rootSignatureDesc, featureData.HighestVersion, &signature, &error);
        if (FAILED(hr))
        {
            PrintBlob(error.Get());
            exit(1);
        }
        hr = (pDevice->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&pRootSigniture)));
        if (FAILED(hr))
        {
            PrintBlob(error.Get());
            exit(1);
        }
    }

    void init(ID3D12Device* pDevice, CD3DX12_ROOT_PARAMETER1* rootParameters, UINT numRootParameters, D3D12_ROOT_SIGNATURE_FLAGS flags)
    {

        CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc;
        rootSignatureDesc.Init_1_1(numRootParameters, rootParameters, 0, nullptr, flags);

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
        D3D12_INPUT_ELEMENT_DESC* inputElementDescs, UINT inputElementSize, D3D12_PRIMITIVE_TOPOLOGY_TYPE topologyType)
    {
        ComPtr<ID3DBlob> vertexShader;
        ComPtr<ID3DBlob> pixelShader;

#if defined(_DEBUG)
        // Enable better shader debugging with the graphics debugging tools.
        UINT compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
        UINT compileFlags = 0;
#endif
        ID3DBlob* shaderCompileErrorsBlob;
        HRESULT hResult = D3DCompileFromFile((LPCWSTR)vertexShaderPath, nullptr, nullptr, "VSMain", "vs_5_0", compileFlags, 0, &vertexShader, &shaderCompileErrorsBlob);

        if (FAILED(hResult))
        {
            const char* errorString = NULL;
            if (hResult == HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND))
                errorString = "could not compile, file not found.";
            else if (shaderCompileErrorsBlob)
            {
                errorString = (const char*)shaderCompileErrorsBlob->GetBufferPointer();
                shaderCompileErrorsBlob->Release();
            }
            MessageBoxA(0, errorString, "Shader compile error", MB_ICONERROR | MB_OK);
        }

        ThrowIfFailed(D3DCompileFromFile((LPCWSTR)pixelShaderPath, nullptr, nullptr, "PSMain", "ps_5_0", compileFlags, 0, &pixelShader, &shaderCompileErrorsBlob));
        if (FAILED(hResult))
        {
            const char* errorString = NULL;
            if (hResult == HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND))
                errorString = "could not compile, file not found.";
            else if (shaderCompileErrorsBlob)
            {
                errorString = (const char*)shaderCompileErrorsBlob->GetBufferPointer();
                shaderCompileErrorsBlob->Release();
            }
            MessageBoxA(0, errorString, "Shader compile error", MB_ICONERROR | MB_OK);
        }

        // Describe and create the graphics pipeline state object (PSO).
        D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
        psoDesc.InputLayout = { inputElementDescs, inputElementSize };
        psoDesc.pRootSignature = rootSigniture.pRootSigniture;
        psoDesc.VS = CD3DX12_SHADER_BYTECODE(vertexShader.Get());
        psoDesc.PS = CD3DX12_SHADER_BYTECODE(pixelShader.Get());

        if (topologyType == D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE)
        {
            psoDesc.RasterizerState.FillMode = D3D12_FILL_MODE_WIREFRAME; // Draw lines as wireframe
            psoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE; // Disable back-face culling
        }
        else
        {
            psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
            psoDesc.RasterizerState.FrontCounterClockwise = TRUE;
            psoDesc.RasterizerState.DepthBiasClamp = 0.0f;
            psoDesc.RasterizerState.DepthBias = 1000;
            psoDesc.RasterizerState.SlopeScaledDepthBias = 1.0f;
            psoDesc.RasterizerState.DepthClipEnable = TRUE;
        }

        psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
        psoDesc.BlendState.RenderTarget[0].BlendEnable = TRUE;
        psoDesc.BlendState.RenderTarget[0].LogicOpEnable = FALSE;
        psoDesc.BlendState.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
        psoDesc.BlendState.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
        psoDesc.BlendState.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
        psoDesc.BlendState.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
        psoDesc.BlendState.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
        psoDesc.BlendState.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
        psoDesc.BlendState.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
        CD3DX12_DEPTH_STENCIL_DESC1 depthDesc(D3D12_DEFAULT);
        depthDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
        depthDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
        psoDesc.DepthStencilState = depthDesc;
        psoDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
        psoDesc.SampleMask = UINT_MAX;
        psoDesc.PrimitiveTopologyType = topologyType;
        psoDesc.NumRenderTargets = 1;
        psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
        psoDesc.SampleDesc.Count = 1;

        ThrowIfFailed(pDevice->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&pPipelineState)));
    }

    void initShadows(ID3D12Device* pDevice, RootSigniture& rootSigniture, LPCWSTR vertexShaderPath, LPCWSTR pixelShaderPath,
        D3D12_INPUT_ELEMENT_DESC* inputElementDescs, UINT inputElementSize, D3D12_PRIMITIVE_TOPOLOGY_TYPE topologyType)
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

        // Describe and create the graphics pipeline state object (PSO)
        D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
        psoDesc.InputLayout = { inputElementDescs, inputElementSize };
        psoDesc.pRootSignature = rootSigniture.pRootSigniture;
        psoDesc.VS = CD3DX12_SHADER_BYTECODE(vertexShader.Get());
        psoDesc.PS = CD3DX12_SHADER_BYTECODE(pixelShader.Get()); // null shader
        psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
        psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
        psoDesc.DepthStencilState.DepthEnable = TRUE;
        psoDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
        psoDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
        psoDesc.DepthStencilState.StencilEnable = FALSE;
        psoDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
        psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
        psoDesc.NumRenderTargets = 0; // No render targets
        psoDesc.SampleMask = UINT_MAX;
        psoDesc.SampleDesc.Count = 1;

        ThrowIfFailed(pDevice->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&pPipelineState)));
    }

};
