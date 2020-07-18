#include <WICTextureLoader.h>

#include "logger.h"
#include "model_resource.h"
#include "misc.h"
#include"texture.h"

ModelResource::ModelResource(ID3D11Device* device, std::unique_ptr<ModelData> data, SHADER_TYPE shaderType)
{
	m_data = std::move(data);

	// マテリアル
	diffuses.resize(m_data->diffuses.size());
	normals.resize(m_data->normals.size());
	bumps.resize(m_data->bumps.size());
	mShaderType = shaderType;
	//テクスチャをロードする処理
	struct
	{
		Material operator()(ID3D11Device*device,std::string textureFileName, const VECTOR4F& color)
		{
			Material material;
			material.color = color;

			if (!textureFileName.empty())
			{
				size_t length;
				wchar_t filename[256];
				::mbstowcs_s(&length, filename, 256, textureFileName.c_str(), _TRUNCATE);
				D3D11_TEXTURE2D_DESC texture2dDesc;
				HRESULT hr = load_texture_from_file(device, filename, material.shader_resource_view.GetAddressOf(), &texture2dDesc);
				_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
			}
			return material;
		}
	}texture;
	for (size_t material_index = 0; material_index < diffuses.size(); ++material_index)
	{
		diffuses.at(material_index) = texture(device, m_data->diffuses.at(material_index).texture_filename, m_data->diffuses.at(material_index).color);
		if (shaderType == SHADER_TYPE::NORMAL)
		{
			normals.at(material_index) = texture(device, m_data->normals.at(material_index).texture_filename, m_data->normals.at(material_index).color);
			bumps.at(material_index) = texture(device, m_data->bumps.at(material_index).texture_filename, m_data->bumps.at(material_index).color);
		}
	}

	// メッシュ
	m_meshes.resize(m_data->meshes.size());
	for (size_t mesh_index = 0; mesh_index < m_meshes.size(); ++mesh_index)
	{
		auto&& src = m_data->meshes.at(mesh_index);
		auto&& dst = m_meshes.at(mesh_index);

		// 頂点バッファ
		{
			D3D11_BUFFER_DESC buffer_desc = {};
			D3D11_SUBRESOURCE_DATA subresource_data = {};

			buffer_desc.ByteWidth = static_cast<UINT>(sizeof(ModelData::Vertex) * src.vertices.size());
			//buffer_desc.Usage = D3D11_USAGE_DEFAULT;
			buffer_desc.Usage = D3D11_USAGE_IMMUTABLE;
			buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			buffer_desc.CPUAccessFlags = 0;
			buffer_desc.MiscFlags = 0;
			buffer_desc.StructureByteStride = 0;
			subresource_data.pSysMem = src.vertices.data();
			subresource_data.SysMemPitch = 0;
			subresource_data.SysMemSlicePitch = 0;

			HRESULT hr = device->CreateBuffer(&buffer_desc, &subresource_data, dst.vertex_buffer.GetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		}

		// インデックスバッファ
		{
			D3D11_BUFFER_DESC buffer_desc = {};
			D3D11_SUBRESOURCE_DATA subresource_data = {};

			buffer_desc.ByteWidth = static_cast<UINT>(sizeof(u_int) * src.indices.size());
			//buffer_desc.Usage = D3D11_USAGE_DEFAULT;
			buffer_desc.Usage = D3D11_USAGE_IMMUTABLE;
			buffer_desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
			buffer_desc.CPUAccessFlags = 0;
			buffer_desc.MiscFlags = 0;
			buffer_desc.StructureByteStride = 0;
			subresource_data.pSysMem = src.indices.data();
			subresource_data.SysMemPitch = 0; //Not use for index buffers.
			subresource_data.SysMemSlicePitch = 0; //Not use for index buffers.
			HRESULT hr = device->CreateBuffer(&buffer_desc, &subresource_data, dst.index_buffer.GetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		}

		dst.node_index = src.node_index;

		// サブセット
		dst.subsets.resize(src.subsets.size());
		for (size_t subset_index = 0; subset_index < src.subsets.size(); ++subset_index)
		{
			auto&& src_subset = src.subsets.at(subset_index);
			auto&& dst_subset = dst.subsets.at(subset_index);

			dst_subset.start_index = src_subset.start_index;
			dst_subset.index_count = src_subset.index_count;
			dst_subset.diffuse = &diffuses.at(src_subset.material_index);
			//if (shaderType == SHADER_TYPE::NORMAL)
			//{
				dst_subset.normal = &normals.at(src_subset.material_index);
				dst_subset.bump = &bumps.at(src_subset.material_index);
			//}
		}

		// ボーン変換行列用
		dst.node_indices.resize(src.node_indices.size());
		::memcpy(dst.node_indices.data(), src.node_indices.data(), sizeof(int) * dst.node_indices.size());

		dst.inverse_transforms.resize(src.inverse_transforms.size());
		for (size_t index = 0; index < dst.inverse_transforms.size(); ++index)
		{
			dst.inverse_transforms.at(index) = &src.inverse_transforms.at(index);
		}
	}
}
