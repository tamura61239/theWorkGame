#include "logger.h"
#include "model_renderer.h"
#include "misc.h"
#include"shader.h"
#include"camera_manager.h"

ModelRenderer::ModelRenderer(ID3D11Device* device)
{
	// ���̓��C�A�E�g
	D3D11_INPUT_ELEMENT_DESC input_element_desc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT,    0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,       0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "WEIGHTS",  0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "BONES",    0, DXGI_FORMAT_R32G32B32A32_UINT,  0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	mShader.push_back(std::make_unique<DrowShader>(device, "Data/shader/model_vs.cso", "", "Data/shader/model_ps.cso", input_element_desc, ARRAYSIZE(input_element_desc)));
	mShader.push_back(std::make_unique<DrowShader>(device, "Data/shader/model_normal_vs.cso", "", "Data/shader/model_normal_ps.cso", input_element_desc, ARRAYSIZE(input_element_desc)));
	mShader.push_back(std::make_unique<DrowShader>(device, "Data/shader/model_motion_data_vs.cso", "Data/shader/model_motion_data_gs.cso", "Data/shader/model_motion_data_ps.cso", input_element_desc, ARRAYSIZE(input_element_desc)));
	mShadowShader = std::make_unique<DrowShader>(device, "Data/shader/model_shadow_vs.cso", "", "", input_element_desc, ARRAYSIZE(input_element_desc));
	// �萔�o�b�t�@
	{
		mCbScene = std::make_unique<ConstantBuffer<CbScene>>(device);
		mCbMesh = std::make_unique<ConstantBuffer<CbMesh>>(device);
		mCbSubset = std::make_unique<ConstantBuffer<CbSubset>>(device);
	}


	//// �[�x�X�e���V���X�e�[�g
	//{
	//	D3D11_DEPTH_STENCIL_DESC desc;
	//	::memset(&desc, 0, sizeof(desc));
	//	desc.DepthEnable = true;
	//	desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	//	desc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

	//	HRESULT hr = device->CreateDepthStencilState(&desc, mDepthStencilState.GetAddressOf());
	//	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	//}

	//// ���X�^���C�U�[�X�e�[�g
	//{
	//	D3D11_RASTERIZER_DESC desc;
	//	::memset(&desc, 0, sizeof(desc));
	//	desc.FrontCounterClockwise = true;
	//	desc.DepthBias = 0;
	//	desc.DepthBiasClamp = 0;
	//	desc.SlopeScaledDepthBias = 0;
	//	desc.DepthClipEnable = true;
	//	desc.ScissorEnable = false;
	//	desc.MultisampleEnable = true;
	//	desc.FillMode = D3D11_FILL_SOLID;
	//	desc.CullMode = D3D11_CULL_NONE;
	//	desc.AntialiasedLineEnable = false;

	//	HRESULT hr = device->CreateRasterizerState(&desc, mRasterizerState.GetAddressOf());
	//	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	//}

	//// �T���v���X�e�[�g
	//{
	//	D3D11_SAMPLER_DESC desc;
	//	::memset(&desc, 0, sizeof(desc));
	//	desc.MipLODBias = 0.0f;
	//	desc.MaxAnisotropy = 1;
	//	desc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	//	desc.MinLOD = -FLT_MAX;
	//	desc.MaxLOD = FLT_MAX;
	//	desc.BorderColor[0] = .0f;
	//	desc.BorderColor[1] = .0f;
	//	desc.BorderColor[2] = .0f;
	//	desc.BorderColor[3] = .0f;
	//	desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	//	desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	//	desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	//	desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;

	//	HRESULT hr = device->CreateSamplerState(&desc, mSamplerState[0].GetAddressOf());
	//	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	//	desc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
	//	desc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	//	desc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;

	//	hr = device->CreateSamplerState(&desc, mSamplerState[1].GetAddressOf());
	//	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	//}

	// �_�~�[�e�N�X�`��
	{
		const int width = 8;
		const int height = 8;
		UINT pixels[width * height];
		::memset(pixels, 0xFF, sizeof(pixels));

		D3D11_TEXTURE2D_DESC desc = { 0 };
		desc.Width = width;
		desc.Height = height;
		desc.MipLevels = 1;
		desc.ArraySize = 1;
		desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		desc.SampleDesc.Count = 1;
		desc.SampleDesc.Quality = 0;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = 0;
		D3D11_SUBRESOURCE_DATA data;
		::memset(&data, 0, sizeof(data));
		data.pSysMem = pixels;
		data.SysMemPitch = width;

		Microsoft::WRL::ComPtr<ID3D11Texture2D>	texture;
		HRESULT hr = device->CreateTexture2D(&desc, &data, texture.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

		hr = device->CreateShaderResourceView(texture.Get(), nullptr, mDummySRV.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}
}

// �`��J�n
void ModelRenderer::Begin(ID3D11DeviceContext* context, const FLOAT4X4& view_projection)
{



	// �V�[���p�萔�o�b�t�@�X�V
	CbScene cb_scene;
	mCbScene->data.viewProjection = view_projection;

	mCbScene->Activate(context, 0, true, true, true);
}

// �`��
void ModelRenderer::Draw(ID3D11DeviceContext* context, Model& model, const VECTOR4F& color)
{
	const ModelResource* model_resource = model.GetModelResource();
	const std::vector<Model::Node>& nodes = model.GetNodes();

	SHADER_TYPE shaderType = model_resource->GetShaderType();
	mShader[static_cast<int>(shaderType)]->Activate(context);
	for (const ModelResource::Mesh& mesh : model_resource->GetMeshes())
	{
		// ���b�V���p�萔�o�b�t�@�X�V
		if (mesh.nodeIndices.size() > 0)
		{
			for (size_t i = 0; i < mesh.nodeIndices.size(); ++i)
			{
				DirectX::XMMATRIX inverse_transform = DirectX::XMLoadFloat4x4(mesh.inverseTransforms.at(i));
				DirectX::XMMATRIX world_transform = DirectX::XMLoadFloat4x4(&nodes.at(mesh.nodeIndices.at(i)).worldTransform);
				DirectX::XMMATRIX bone_transform = inverse_transform * world_transform;
				DirectX::XMStoreFloat4x4(&mCbMesh->data.boneTransforms[i], bone_transform);
				DirectX::XMMATRIX beforeWorldTransform = DirectX::XMLoadFloat4x4(&nodes.at(mesh.nodeIndices.at(i)).beforeWorldTransform);
				bone_transform = inverse_transform * beforeWorldTransform;
				DirectX::XMStoreFloat4x4(&mCbMesh->data.beforeBoneTransforms[i], bone_transform);

			}
		}
		else
		{
			mCbMesh->data.boneTransforms[0] = nodes.at(mesh.nodeIndex).worldTransform;
			mCbMesh->data.beforeBoneTransforms[0] = nodes.at(mesh.nodeIndex).beforeWorldTransform;
		}
		mCbMesh->Activate(context, 1, true, true, true);

		UINT stride = sizeof(ModelData::Vertex);
		UINT offset = 0;
		context->IASetVertexBuffers(0, 1, mesh.vertexBuffer.GetAddressOf(), &stride, &offset);
		context->IASetIndexBuffer(mesh.indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
		context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		for (const ModelResource::Subset& subset : mesh.subsets)
		{
			mCbSubset->data.materialColor = VECTOR4F(subset.diffuse->color.x * color.x, subset.diffuse->color.y * color.y, subset.diffuse->color.z * color.z, subset.diffuse->color.w * color.w);
			mCbSubset->Activate(context, 2, true, true, true);
			context->PSSetShaderResources(0, 1, subset.diffuse->SRV.Get() ? subset.diffuse->SRV.GetAddressOf() : mDummySRV.GetAddressOf());
			if (shaderType == SHADER_TYPE::NORMAL)
			{
				context->PSSetShaderResources(1, 1, subset.normal->SRV.Get() ? subset.normal->SRV.GetAddressOf() : mDummySRV.GetAddressOf());
				context->PSSetShaderResources(2, 1, subset.bump->SRV.Get() ? subset.bump->SRV.GetAddressOf() : mDummySRV.GetAddressOf());
			}
			context->DrawIndexed(subset.indexCount, subset.startIndex, 0);
			mCbSubset->DeActivate(context);
		}
		mCbMesh->DeActivate(context);
	}
	mShader[static_cast<int>(shaderType)]->Deactivate(context);
}

void ModelRenderer::Draw(ID3D11DeviceContext* context, DrowShader* shader, Model& model, const VECTOR4F& color)
{
	const ModelResource* model_resource = model.GetModelResource();
	const std::vector<Model::Node>& nodes = model.GetNodes();

	SHADER_TYPE shaderType = model_resource->GetShaderType();
	shader->Activate(context);
	for (const ModelResource::Mesh& mesh : model_resource->GetMeshes())
	{
		// ���b�V���p�萔�o�b�t�@�X�V
		if (mesh.nodeIndices.size() > 0)
		{
			for (size_t i = 0; i < mesh.nodeIndices.size(); ++i)
			{
				DirectX::XMMATRIX inverse_transform = DirectX::XMLoadFloat4x4(mesh.inverseTransforms.at(i));
				DirectX::XMMATRIX world_transform = DirectX::XMLoadFloat4x4(&nodes.at(mesh.nodeIndices.at(i)).worldTransform);
				DirectX::XMMATRIX bone_transform = inverse_transform * world_transform;
				DirectX::XMStoreFloat4x4(&mCbMesh->data.boneTransforms[i], bone_transform);
				DirectX::XMMATRIX beforeWorldTransform = DirectX::XMLoadFloat4x4(&nodes.at(mesh.nodeIndices.at(i)).beforeWorldTransform);
				bone_transform = inverse_transform * beforeWorldTransform;
				DirectX::XMStoreFloat4x4(&mCbMesh->data.beforeBoneTransforms[i], bone_transform);

			}
		}
		else
		{
			mCbMesh->data.boneTransforms[0] = nodes.at(mesh.nodeIndex).worldTransform;
			mCbMesh->data.beforeBoneTransforms[0] = nodes.at(mesh.nodeIndex).beforeWorldTransform;
		}
		mCbMesh->Activate(context, 1, true, true, true);

		UINT stride = sizeof(ModelData::Vertex);
		UINT offset = 0;
		context->IASetVertexBuffers(0, 1, mesh.vertexBuffer.GetAddressOf(), &stride, &offset);
		context->IASetIndexBuffer(mesh.indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
		context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		for (const ModelResource::Subset& subset : mesh.subsets)
		{
			mCbSubset->data.materialColor = VECTOR4F(subset.diffuse->color.x * color.x, subset.diffuse->color.y * color.y, subset.diffuse->color.z * color.z, subset.diffuse->color.w * color.w);
			mCbSubset->Activate(context, 2, true, true, true);
			context->PSSetShaderResources(0, 1, subset.diffuse->SRV.Get() ? subset.diffuse->SRV.GetAddressOf() : mDummySRV.GetAddressOf());
			if (shaderType == SHADER_TYPE::NORMAL)
			{
				context->PSSetShaderResources(1, 1, subset.normal->SRV.Get() ? subset.normal->SRV.GetAddressOf() : mDummySRV.GetAddressOf());
				context->PSSetShaderResources(2, 1, subset.bump->SRV.Get() ? subset.bump->SRV.GetAddressOf() : mDummySRV.GetAddressOf());
			}
			context->DrawIndexed(subset.indexCount, subset.startIndex, 0);
			mCbSubset->DeActivate(context);
		}
		mCbMesh->DeActivate(context);

	}
	shader->Deactivate(context);

}

// �`��I��
void ModelRenderer::End(ID3D11DeviceContext* context)
{
	//context->GSSetShader(d_m_g_shader.Get(), 0, 0);
	mCbScene->DeActivate(context);
	//context->OMSetDepthStencilState(nullptr, 0);
	//context->RSSetState(nullptr);


}
/*********************************************************************/
//    �e�̕`��
/*********************************************************************/
void ModelRenderer::ShadowBegin(ID3D11DeviceContext* context, const FLOAT4X4& view_projection)
{
	mShadowShader->Activate(context);


	// �V�[���p�萔�o�b�t�@�X�V
	mCbScene->data.viewProjection = view_projection;

	mCbScene->Activate(context, 0, true, true);

}

void ModelRenderer::ShadowDraw(ID3D11DeviceContext* context, Model& model, const VECTOR4F& color)
{
	const ModelResource* model_resource = model.GetModelResource();
	const std::vector<Model::Node>& nodes = model.GetNodes();

	for (const ModelResource::Mesh& mesh : model_resource->GetMeshes())
	{
		// ���b�V���p�萔�o�b�t�@�X�V
		if (mesh.nodeIndices.size() > 0)
		{
			for (size_t i = 0; i < mesh.nodeIndices.size(); ++i)
			{
				DirectX::XMMATRIX inverse_transform = DirectX::XMLoadFloat4x4(mesh.inverseTransforms.at(i));
				DirectX::XMMATRIX world_transform = DirectX::XMLoadFloat4x4(&nodes.at(mesh.nodeIndices.at(i)).worldTransform);
				DirectX::XMMATRIX bone_transform = inverse_transform * world_transform;
				DirectX::XMStoreFloat4x4(&mCbMesh->data.boneTransforms[i], bone_transform);
				DirectX::XMMATRIX beforeWorldTransform = DirectX::XMLoadFloat4x4(&nodes.at(mesh.nodeIndices.at(i)).beforeWorldTransform);
				bone_transform = inverse_transform * beforeWorldTransform;
				DirectX::XMStoreFloat4x4(&mCbMesh->data.beforeBoneTransforms[i], bone_transform);

			}
		}
		else
		{
			mCbMesh->data.boneTransforms[0] = nodes.at(mesh.nodeIndex).worldTransform;
			mCbMesh->data.beforeBoneTransforms[0] = nodes.at(mesh.nodeIndex).beforeWorldTransform;
		}
		mCbMesh->Activate(context, 1, true, true);

		UINT stride = sizeof(ModelData::Vertex);
		UINT offset = 0;
		context->IASetVertexBuffers(0, 1, mesh.vertexBuffer.GetAddressOf(), &stride, &offset);
		context->IASetIndexBuffer(mesh.indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
		context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		for (const ModelResource::Subset& subset : mesh.subsets)
		{
			mCbSubset->data.materialColor = VECTOR4F(subset.diffuse->color.x * color.x, subset.diffuse->color.y * color.y, subset.diffuse->color.z * color.z, subset.diffuse->color.w * color.w);
			mCbSubset->Activate(context, 2, true, true);
			context->DrawIndexed(subset.indexCount, subset.startIndex, 0);
			mCbSubset->DeActivate(context);
		}
		mCbMesh->DeActivate(context);
	}

}

void ModelRenderer::ShadowEnd(ID3D11DeviceContext* context)
{
	mShadowShader->Deactivate(context);
	mCbScene->DeActivate(context);
	//context->OMSetDepthStencilState(nullptr, 0);
	//context->RSSetState(nullptr);

}
/*********************************************************************/
//    ���x�}�b�v�̕`��
/*********************************************************************/

void ModelRenderer::VelocityBegin(ID3D11DeviceContext* context, const FLOAT4X4& viewProjection)
{
	mShader[2]->Activate(context);

	// �V�[���p�萔�o�b�t�@�X�V
	mCbScene->data.viewProjection = viewProjection;

	mCbScene->Activate(context, 0, true, true, true);

}

void ModelRenderer::VelocityDraw(ID3D11DeviceContext* context, Model& model)
{
	const ModelResource* model_resource = model.GetModelResource();
	const std::vector<Model::Node>& nodes = model.GetNodes();
	for (const ModelResource::Mesh& mesh : model_resource->GetMeshes())
	{
		// ���b�V���p�萔�o�b�t�@�X�V
		if (mesh.nodeIndices.size() > 0)
		{
			for (size_t i = 0; i < mesh.nodeIndices.size(); ++i)
			{
				DirectX::XMMATRIX inverse_transform = DirectX::XMLoadFloat4x4(mesh.inverseTransforms.at(i));
				DirectX::XMMATRIX world_transform = DirectX::XMLoadFloat4x4(&nodes.at(mesh.nodeIndices.at(i)).worldTransform);
				DirectX::XMMATRIX bone_transform = inverse_transform * world_transform;
				DirectX::XMStoreFloat4x4(&mCbMesh->data.boneTransforms[i], bone_transform);
				DirectX::XMMATRIX beforeWorldTransform = DirectX::XMLoadFloat4x4(&nodes.at(mesh.nodeIndices.at(i)).beforeWorldTransform);
				bone_transform = inverse_transform * beforeWorldTransform;
				DirectX::XMStoreFloat4x4(&mCbMesh->data.beforeBoneTransforms[i], bone_transform);

			}
		}
		else
		{
			mCbMesh->data.boneTransforms[0] = nodes.at(mesh.nodeIndex).worldTransform;
			mCbMesh->data.beforeBoneTransforms[0] = nodes.at(mesh.nodeIndex).beforeWorldTransform;
		}
		mCbMesh->Activate(context, 1, true, true, true);

		UINT stride = sizeof(ModelData::Vertex);
		UINT offset = 0;
		context->IASetVertexBuffers(0, 1, mesh.vertexBuffer.GetAddressOf(), &stride, &offset);
		context->IASetIndexBuffer(mesh.indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
		context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		for (const ModelResource::Subset& subset : mesh.subsets)
		{
			context->DrawIndexed(subset.indexCount, subset.startIndex, 0);
		}
		mCbMesh->DeActivate(context);
	}

}

void ModelRenderer::VelocityEnd(ID3D11DeviceContext* context)
{
	mShader[2]->Deactivate(context);
	mCbScene->DeActivate(context);
	//context->OMSetDepthStencilState(nullptr, 0);
	//context->RSSetState(nullptr);

}
