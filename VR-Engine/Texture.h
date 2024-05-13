#pragma once
struct Texture
{
    ID3D12Resource* m_texture;
    D3D12_GPU_DESCRIPTOR_HANDLE gpuDescriptorHandle;

    void init(ID3D12Device* pDevice, ID3D12GraphicsCommandList* pCommandList, ID3D12Resource* textureUploadHeap, DescriptorHandleProvider& dhp,
        UINT texWidth, UINT texHeight, unsigned char* texBytes)
    {
        // Create the texture.
        {
            // Describe and create a Texture2D.
            D3D12_RESOURCE_DESC textureDesc = {};
            textureDesc.MipLevels = 1;
            textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
            textureDesc.Width = texWidth;
            textureDesc.Height = texHeight;
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



            D3D12_SUBRESOURCE_DATA textureData = {};
            textureData.pData = texBytes;
            textureData.RowPitch = texWidth * 4;
            textureData.SlicePitch = textureData.RowPitch * texHeight;

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