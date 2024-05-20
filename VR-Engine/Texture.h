#pragma once
#include "stb_image.h"

struct Texture
{
	ID3D12Resource* m_texture;
	D3D12_GPU_DESCRIPTOR_HANDLE gpuDescriptorHandle;

	virtual void init(ID3D12Device* pDevice, ID3D12GraphicsCommandList* pCommandList, ID3D12Resource* textureUploadHeap, DescriptorHandleProvider& dhp,
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

	void expandRGBToRGBA(const uint8_t* rgbData, uint32_t width, uint32_t height, std::vector<uint8_t>& rgbaData) {
		rgbaData.resize(width * height * 4);
		for (uint32_t i = 0, j = 0; i < width * height * 3; i += 3, j += 4) {
			rgbaData[j] = rgbData[i];       // R
			rgbaData[j + 1] = rgbData[i + 1]; // G
			rgbaData[j + 2] = rgbData[i + 2]; // B
			rgbaData[j + 3] = 255;          // A (fully opaque)
		}
	}


	virtual void init(ID3D12Device* pDevice, ID3D12GraphicsCommandList* pCommandList, ID3D12Resource* textureUploadHeap, DescriptorHandleProvider& dhp,
		const char* filePath)
	{
		int width, height, channels = 0;
		unsigned char* data = stbi_load(filePath, &width, &height, &channels, 4);
		if (channels > 0)
		{
			//if (channels == 3)
			//{
			//    std::vector<uint8_t> rgbaData;
			//    expandRGBToRGBA(data, width, height, rgbaData);
			//    this->init(pDevice, pCommandList, textureUploadHeap, dhp, width, height, rgbaData.data());
			//}
			//else
			//{
			this->init(pDevice, pCommandList, textureUploadHeap, dhp, width, height, data);
			//}
			free(data);
		}
	}




	void bind(ID3D12GraphicsCommandList* pCommandList, UINT rootParameterIndex)
	{
		pCommandList->SetGraphicsRootDescriptorTable(rootParameterIndex, gpuDescriptorHandle);
	}

#pragma pack(push, 1)
	struct BMPFileHeader {
		uint16_t fileType{ 0x4D42 };       // File type always BM which is 0x4D42
		uint32_t fileSize{ 0 };            // Size of the file (in bytes)
		uint16_t reserved1{ 0 };           // Reserved, always 0
		uint16_t reserved2{ 0 };           // Reserved, always 0
		uint32_t offsetData{ 0 };          // Start position of pixel data (bytes from the beginning of the file)
	};

	struct BMPInfoHeader {
		uint32_t size{ 0 };                // Size of this header (in bytes)
		int32_t width{ 0 };                // width of bitmap in pixels
		int32_t height{ 0 };               // width of bitmap in pixels
		// (if positive, bottom-up, with origin in lower left corner)
		// (if negative, top-down, with origin in upper left corner)
		uint16_t planes{ 1 };              // No. of planes for the target device, this is always 1
		uint16_t bitCount{ 0 };            // No. of bits per pixel
		uint32_t compression{ 0 };         // 0 or 3 - uncompressed. This program only uses uncompressed BMPs
		uint32_t sizeImage{ 0 };           // 0 - for uncompressed images
		int32_t xPixelsPerMeter{ 0 };
		int32_t yPixelsPerMeter{ 0 };
		uint32_t colorsUsed{ 0 };          // No. color indexes in the color table. Use 0 for the max number of colors allowed by bit_count
		uint32_t colorsImportant{ 0 };     // No. of colors used for displaying the bitmap. If 0 all colors are required
	};
#pragma pack(pop)

	static void writeBMP(const char* filename, const unsigned char* data, int width, int height) {
		// Create BMP file header
		BMPFileHeader fileHeader;
		BMPInfoHeader infoHeader;

		int rowStride = width * 3;
		int paddingSize = (4 - (rowStride) % 4) % 4;

		fileHeader.fileSize = sizeof(BMPFileHeader) + sizeof(BMPInfoHeader) + (rowStride + paddingSize) * height;
		fileHeader.offsetData = sizeof(BMPFileHeader) + sizeof(BMPInfoHeader);

		infoHeader.size = sizeof(BMPInfoHeader);
		infoHeader.width = width;
		infoHeader.height = height;
		infoHeader.bitCount = 24;
		infoHeader.sizeImage = (rowStride + paddingSize) * height;

		std::ofstream of{ filename, std::ios::binary };
		if (!of) {
			//std::cerr << "Could not open file for writing: " << filename << std::endl;
			return;
		}

		// Write the file headers
		of.write(reinterpret_cast<const char*>(&fileHeader), sizeof(fileHeader));
		of.write(reinterpret_cast<const char*>(&infoHeader), sizeof(infoHeader));

		// Write the pixel data
		for (int y = 0; y < height; ++y) {
			of.write(reinterpret_cast<const char*>(data + (width * 3 * y)), rowStride);
			of.write("\0\0\0", paddingSize);  // padding
		}
	}
};


struct TextureAtlas : Texture
{

	struct AtlasComponent
	{
		UINT width;
		UINT height;
		UINT atlasStartHorizontal;

		AtlasComponent(UINT width, UINT height, UINT atlasStartHorizontal)
		{
			this->width = width;
			this->height = height;
			this->atlasStartHorizontal = atlasStartHorizontal;
		}
		AtlasComponent()
		{
			this->width = 0;
			this->height = 0;
			this->atlasStartHorizontal = 0;
		}
	};

	std::map<std::string, AtlasComponent> componentMap;

	UINT fullWidth, fullHeight;

	std::vector<uint8_t> loadTextureAsVector(const char* filePath, int* pWidth, int* pHeight)
	{
		int channels;
		unsigned char* data = stbi_load(filePath, pWidth, pHeight, &channels, 4); // Force 4 channels (RGBA)

		if (!data)
		{
			std::cerr << "Failed to load image: " << filePath << std::endl;
			return std::vector<uint8_t>();
		}

		std::vector<uint8_t> rgbaData(data, data + *pWidth * *pHeight * 4);

		stbi_image_free(data);
		return rgbaData;
	}


	std::vector<uint8_t> combineTextures(const std::vector<uint8_t>& texture1,
		int width1, int height1,
		const std::vector<uint8_t>& texture2,
		int width2, int height2, UINT *pMaxHeight, UINT *pCombinedWidth) 
	{
		int combinedWidth = width1 + width2;
		int combinedHeight = max(height1, height2);

		std::vector<uint8_t> combinedTexture(combinedWidth * combinedHeight * 4);

		for (int y = 0; y < height1; ++y) {
			for (int x = 0; x < width1; ++x) {
				int srcIndex = (y * width1 + x) * 4;
				int dstIndex = (y * combinedWidth + x) * 4;
				for (int i = 0; i < 4; ++i) {
					combinedTexture[dstIndex + i] = texture1[srcIndex + i];
				}
			}
		}

		for (int y = 0; y < height2; ++y) {
			for (int x = 0; x < width2; ++x) {
				int srcIndex = (y * width2 + x) * 4;
				int dstIndex = ((y * combinedWidth) + width1 + x) * 4;
				for (int i = 0; i < 4; ++i) {
					combinedTexture[dstIndex + i] = texture2[srcIndex + i];
				}
			}
		}
		*pCombinedWidth = combinedWidth;
		*pMaxHeight = combinedHeight;
		return combinedTexture;
	}

	void init(ID3D12Device* pDevice, ID3D12GraphicsCommandList* pCommandList, ID3D12Resource* textureUploadHeap, DescriptorHandleProvider& dhp,
		std::vector<std::pair<std::string, std::string>> paths)
	{

		std::vector<UINT> widths, heights;
		UINT maxHeight = 0;
		std::vector<uint8_t> atlasData;

		for (int i = 0; i < paths.size(); i++)
		{
			int width, height, channels = 0;
			std::vector<uint8_t> data = loadTextureAsVector(paths[i].second.c_str(), &width, &height);
			if (data.size() > 0)
			{
				if (i == 0)
				{
					atlasData = data;
					fullWidth = width;
					fullHeight = height;
					AtlasComponent component(width, height, 0);
					componentMap[paths[i].first] = component;

				}
				else
				{
					AtlasComponent component(width, height, fullWidth);
					componentMap[paths[i].first] = component;
					atlasData = combineTextures(atlasData, fullWidth, fullHeight,
						data, width, height, &fullHeight, &fullWidth);
				}
			}
		}

		Texture::init(pDevice, pCommandList, textureUploadHeap, dhp, fullWidth, fullHeight, atlasData.data());
	}
};