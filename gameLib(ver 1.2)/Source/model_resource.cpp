
#include "logger.h"
#include "model_resource.h"
#include "misc.h"
#include"texture.h"
/*****************************************************/
//　　　　　　　　　　初期化関数(コンストラクタ)
/*****************************************************/

ModelResource::ModelResource(ID3D11Device* device, std::unique_ptr<ModelData> data, SHADER_TYPE shaderType)
{
	mData = std::move(data);

	// マテリアル
	mDiffuses.resize(mData->diffuses.size());
	mNormals.resize(mData->normals.size());
	mBumps.resize(mData->bumps.size());
	mShaderType = shaderType;
	//テクスチャをロードする処理
	struct
	{
		Material operator()(ID3D11Device* device, std::string textureFileName, const VECTOR4F& color)
		{
			Material material;
			material.color = color;

			if (!textureFileName.empty())
			{
				size_t length;
				wchar_t filename[256];
				::mbstowcs_s(&length, filename, 256, textureFileName.c_str(), _TRUNCATE);
				D3D11_TEXTURE2D_DESC texture2dDesc;
				HRESULT hr = LoadTextureFromFile(device, filename, material.SRV.GetAddressOf(), &texture2dDesc);
				_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
			}
			return material;
		}
	}texture;
	//テクスチャを生成する
	for (size_t material_index = 0; material_index < mDiffuses.size(); ++material_index)
	{
		mDiffuses.at(material_index) = texture(device, mData->diffuses.at(material_index).textureFilename, mData->diffuses.at(material_index).color);
		if (shaderType == SHADER_TYPE::NORMAL)
		{
			mNormals.at(material_index) = texture(device, mData->normals.at(material_index).textureFilename, mData->normals.at(material_index).color);
			mBumps.at(material_index) = texture(device, mData->bumps.at(material_index).textureFilename, mData->bumps.at(material_index).color);
		}
	}

	// メッシュ
	mMeshes.resize(mData->meshes.size());
	for (size_t mesh_index = 0; mesh_index < mMeshes.size(); ++mesh_index)
	{
		auto&& src = mData->meshes.at(mesh_index);
		auto&& dst = mMeshes.at(mesh_index);

		// 頂点バッファ
		{
			D3D11_BUFFER_DESC buffer_desc = {};
			D3D11_SUBRESOURCE_DATA subresource_data = {};

			buffer_desc.ByteWidth = static_cast<UINT>(sizeof(ModelData::Vertex) * src.vertices.size());
			buffer_desc.Usage = D3D11_USAGE_IMMUTABLE;
			buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			buffer_desc.CPUAccessFlags = 0;
			buffer_desc.MiscFlags = 0;
			buffer_desc.StructureByteStride = 0;
			subresource_data.pSysMem = src.vertices.data();
			subresource_data.SysMemPitch = 0;
			subresource_data.SysMemSlicePitch = 0;

			HRESULT hr = device->CreateBuffer(&buffer_desc, &subresource_data, dst.vertexBuffer.GetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		}

		// インデックスバッファ
		{
			D3D11_BUFFER_DESC buffer_desc = {};
			D3D11_SUBRESOURCE_DATA subresource_data = {};

			buffer_desc.ByteWidth = static_cast<UINT>(sizeof(u_int) * src.indices.size());
			buffer_desc.Usage = D3D11_USAGE_IMMUTABLE;
			buffer_desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
			buffer_desc.CPUAccessFlags = 0;
			buffer_desc.MiscFlags = 0;
			buffer_desc.StructureByteStride = 0;
			subresource_data.pSysMem = src.indices.data();
			subresource_data.SysMemPitch = 0; //Not use for index buffers.
			subresource_data.SysMemSlicePitch = 0; //Not use for index buffers.
			HRESULT hr = device->CreateBuffer(&buffer_desc, &subresource_data, dst.indexBuffer.GetAddressOf());
			_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		}

		dst.nodeIndex = src.nodeIndex;

		// サブセット
		dst.subsets.resize(src.subsets.size());
		for (size_t subset_index = 0; subset_index < src.subsets.size(); ++subset_index)
		{
			auto&& src_subset = src.subsets.at(subset_index);
			auto&& dst_subset = dst.subsets.at(subset_index);

			dst_subset.startIndex = src_subset.startIndex;
			dst_subset.indexCount = src_subset.indexCount;
			dst_subset.diffuse = &mDiffuses.at(src_subset.materialIndex);
			dst_subset.normal = &mNormals.at(src_subset.materialIndex);
			dst_subset.bump = &mBumps.at(src_subset.materialIndex);
		}

		// ボーン変換行列用
		dst.nodeIndices.resize(src.nodeIndices.size());
		::memcpy(dst.nodeIndices.data(), src.nodeIndices.data(), sizeof(int) * dst.nodeIndices.size());

		dst.inverseTransforms.resize(src.inverseTransforms.size());
		for (size_t index = 0; index < dst.inverseTransforms.size(); ++index)
		{
			dst.inverseTransforms.at(index) = &src.inverseTransforms.at(index);
		}
	}
}
