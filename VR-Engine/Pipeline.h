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

        ThrowIfFailed(D3DCompileFromFile((LPCWSTR)vertexShaderPath, nullptr, nullptr, "VSMain", "vs_5_0", compileFlags, 0, &vertexShader, nullptr));
        ThrowIfFailed(D3DCompileFromFile((LPCWSTR)pixelShaderPath, nullptr, nullptr, "PSMain", "ps_5_0", compileFlags, 0, &pixelShader, nullptr));

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
        }

        psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
        CD3DX12_DEPTH_STENCIL_DESC1 depthDesc(D3D12_DEFAULT);
        psoDesc.DepthStencilState = depthDesc;
        psoDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
        psoDesc.SampleMask = UINT_MAX;
        psoDesc.PrimitiveTopologyType = topologyType;
        psoDesc.NumRenderTargets = 1;
        psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
        psoDesc.SampleDesc.Count = 1;

        ThrowIfFailed(pDevice->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&pPipelineState)));
    }

};
