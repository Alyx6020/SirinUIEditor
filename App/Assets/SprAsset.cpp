#include "SprAsset.h"

Frame* SprAsset::GetFrame(int group, int frame)
{
	if (group <= m_actionList.size())
	{
		auto& list = m_actionList.at(group);

		if (frame <= list.frameCount)
		{
			return &list.frames.at(frame);
		}
	}
	return nullptr;
}

SprAsset::SprAsset(const std::string& filename, const std::filesystem::path& fullpath, const std::string& data, SprFile* file)
{
	name = filename;
	path = fullpath;

	std::stringstream ss(data);
	ss.read((char*)&m_header, sizeof(m_header));

	m_actionList.resize(m_header.actions);

	uint32_t group = 0;
	uint32_t frame = 0;

	for (auto& iter : m_actionList)
	{
		frame = 0;

		ss.read((char*)&iter.frameCount, sizeof(uint32_t));

		iter.frames.resize(iter.frameCount);

		for (auto& it : iter.frames)
		{
			ss.read((char*)&it, 24);

			// Store Group / Frame
			it.group = group;
			it.frame = frame;

			// Set Size
			it.rect.w = it.width;
			it.rect.h = it.height;

			it.path = path;

			// read image data
			it.data.resize(it.linearSize);
			ss.read((char*)it.data.data(), it.linearSize);

			HRESULT hr;

			DXGI_FORMAT format = DXGI_FORMAT_BC1_UNORM;
			if (std::strncmp(it.format, "DXT1", 4) == 0)
			{
				format = DXGI_FORMAT_BC1_UNORM;
			}
			else if (std::strncmp(it.format, "DXT5", 4) == 0)
			{
				format = DXGI_FORMAT_BC3_UNORM;
			}
			else if (it.format[0] == 21)
			{
				format = DXGI_FORMAT_B8G8R8A8_UNORM;
			}

			// Unpack
			D3D11_TEXTURE2D_DESC desc{};
			desc.Width = it.xwidth;
			desc.Height = it.xheight;
			desc.MipLevels = 1;
			desc.ArraySize = 1;
			desc.Format = format;
			desc.SampleDesc.Count = 1;
			desc.Usage = D3D11_USAGE_DEFAULT;
			desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

			// Gen Header

			DirectX::TexMetadata meta{};
			meta.width = it.xwidth;
			meta.height = it.xheight;
			meta.format = format;
			meta.mipLevels = 0;
			meta.arraySize = 0;
			meta.depth = 0;
			meta.dimension = DirectX::TEX_DIMENSION::TEX_DIMENSION_TEXTURE2D;

			std::string buffer;
			buffer.resize(128);
			size_t resultSize = 0;
			DirectX::EncodeDDSHeader(meta, DirectX::DDS_FLAGS::DDS_FLAGS_NONE, buffer.data(), buffer.size(), resultSize);

			buffer.append(it.data.begin(), it.data.end());

			std::ofstream fs("example.dds", std::ios::out | std::ios::binary | std::ios::trunc);
			fs.write(buffer.data(), buffer.size());
			fs.close();

			// Sub resource data
			D3D11_SUBRESOURCE_DATA texData{};
			texData.pSysMem = (void*)it.data.data();

			size_t rowPitch;
			size_t slicePitch;

			DirectX::ComputePitch(format, it.xwidth, it.xheight, rowPitch, slicePitch);

			texData.SysMemPitch = rowPitch;

			hr = graphics::gfx->pDevice->CreateTexture2D(&desc, &texData, &it.texture);

			D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
			ZeroMemory(&srvDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
			srvDesc.Format = desc.Format;
			srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DMS;
			srvDesc.Texture2D.MostDetailedMip = 0;
			srvDesc.Texture2D.MipLevels = 0;

			hr = graphics::gfx->pDevice->CreateShaderResourceView(it.texture.Get(), &srvDesc, &it.srv);

			frame++;
		}

		group++;
	}
}

std::string SprAsset::GetName()
{
	return name;
}
