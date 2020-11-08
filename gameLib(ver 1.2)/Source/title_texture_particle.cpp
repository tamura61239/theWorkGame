#include "title_texture_particle.h"
#include"texture.h"
#include"misc.h"
#include"shader.h"
#include"framework.h"
#include"camera_manager.h"
#ifdef USE_IMGUI
#include<imgui.h>
#endif


TitleTextureParticle::TitleTextureParticle(ID3D11Device* device) :mMaxCount(0), mCreateFlag(false), mZPos(0.2f), mStartFlag(false)
{
	HRESULT hr;
	{
		D3D11_BUFFER_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.ByteWidth = sizeof(CbCreate);
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = 0;
		desc.StructureByteStride = 0;
		hr = device->CreateBuffer(&desc, nullptr, mCbCreateBuffer.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		desc.ByteWidth = sizeof(Cb);
		hr = device->CreateBuffer(&desc, nullptr, mCbBuffer.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}
	hr = create_cs_from_cso(device, "Data/shader/title_texture_particle_cs.cso", mCSShader.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	hr = create_cs_from_cso(device, "Data/shader/title_texture_particle_create_cs.cso", mCSCreateShader.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	//D3D11_INPUT_ELEMENT_DESC inputElementDesc[] =
	//{
	//	{"POSITION",0,DXGI_FORMAT_R32G32B32A32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
	//	{"ANGLE",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
	//	{"COLOR",0,DXGI_FORMAT_R32G32B32A32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
	//	{"VELOCITY",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
	//	{"SCALE",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
	//};
	D3D11_INPUT_ELEMENT_DESC inputElementDesc[] =
	{
		{"POSITION",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
		{"VELOCITY",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
		{"SPEED",0,DXGI_FORMAT_R32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
		{"COLOR",0,DXGI_FORMAT_R32G32B32A32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
		{"LIFE",0,DXGI_FORMAT_R32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
	};

	//mShader = std::make_unique<DrowShader>(device, "Data/shader/title_texture_particle_vs.cso", "Data/shader/run_particle_gs.cso", "Data/shader/stage_obj_particle_ps.cso", inputElementDesc, ARRAYSIZE(inputElementDesc));
}

void TitleTextureParticle::LoadTextureData(const wchar_t* textureName, VECTOR2F leftTop, VECTOR2F size, VECTOR2F textureSize)
{
	SetData set;
	set.texName = textureName;
	set.data.leftTop = leftTop;
	set.data.size = size;
	set.data.textureSize = textureSize;
	mSet.push_back(set);
	particleCount.push_back((size.x * size.y)/3);
}

void TitleTextureParticle::CreateBuffer(ID3D11Device* device)
{
	mSRVs.resize(mSet.size());
	HRESULT hr;
	mMaxCount = 0;
	for (int i = 0; i < mSet.size(); i++)
	{
		hr = load_texture_from_file(device, mSet[i].texName.c_str(), mSRVs[i].GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		mMaxCount += particleCount[i];
	}
	if (mMaxCount <= 0)return;
	mCreateFlag = true;
	//バッファ
	{
		D3D11_BUFFER_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.ByteWidth = sizeof(RenderParticle) * mMaxCount;
		desc.CPUAccessFlags = 0;
		desc.StructureByteStride = 0;
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER | D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
		desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS;
		hr = device->CreateBuffer(&desc, nullptr, mRenderBuffer.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		desc.ByteWidth = sizeof(ParticleData) * mMaxCount;
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER | D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
		hr = device->CreateBuffer(&desc, nullptr, buffer.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	}
	//UAV
	{
		D3D11_UNORDERED_ACCESS_VIEW_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.Format = DXGI_FORMAT_R32_TYPELESS;
		desc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
		desc.Buffer.FirstElement = 0;
		desc.Buffer.Flags = D3D11_BUFFEREX_SRV_FLAG_RAW;
		desc.Buffer.NumElements = sizeof(RenderParticle) * mMaxCount / 4;
		hr = device->CreateUnorderedAccessView(mRenderBuffer.Get(), &desc,mRenderUAV.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		desc.Buffer.NumElements = sizeof(ParticleData) * mMaxCount / 4;
		hr = device->CreateUnorderedAccessView(buffer.Get(), &desc, mUAV.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	}
}

void TitleTextureParticle::Update(float elapsdTime, ID3D11DeviceContext* context)
{
	if (!mStartFlag)return;

	context->CSSetUnorderedAccessViews(0, 1, mUAV.GetAddressOf(), nullptr);
	if (mCreateFlag)
	{
		context->CSSetShader(mCSCreateShader.Get(), nullptr, 0);
		context->CSSetConstantBuffers(0, 1, mCbCreateBuffer.GetAddressOf());
		CbCreate create;
		DirectX::XMStoreFloat4x4(&create.inverseViewProjection, DirectX::XMMatrixInverse(nullptr, DirectX::XMLoadFloat4x4(&pCameraManager.GetCamera()->GetView()) * DirectX::XMLoadFloat4x4(&pCameraManager.GetCamera()->GetProjection())));
		D3D11_VIEWPORT viewport;
		UINT num_viewports = 1;
		context->RSGetViewports(&num_viewports, &viewport);
		create.viewport = VECTOR2F(viewport.Width, viewport.Height);
		create.z = mZPos;

		for (int i = 0; i < mSet.size(); i++)
		{
			context->CSSetShaderResources(0, 1, mSRVs[i].GetAddressOf());
			
			create.textureSize = mSet[i].data.textureSize;
			create.leftTop = mSet[i].data.leftTop;
			if (i == 0)create.startIndex = 0;
			else create.startIndex = particleCount[i - 1];
			context->UpdateSubresource(mCbCreateBuffer.Get(), 0, 0, &create, 0, 0);
			context->Dispatch(particleCount[i], 1, 1);
		}
		mCreateFlag = false;
		ID3D11ShaderResourceView* srv = nullptr;
		context->CSSetShaderResources(0, 1, &srv);

	}
	context->CSSetUnorderedAccessViews(2, 1, mRenderUAV.GetAddressOf(), nullptr);
	context->CSSetConstantBuffers(1, 1, mCbBuffer.GetAddressOf());
	Cb cb;
	cb.elapsdTime = elapsdTime;
	cb.endPosition = VECTOR3F(0, 0, 0);
	context->UpdateSubresource(mCbBuffer.Get(), 0, 0, &cb, 0, 0);

	context->CSSetShader(mCSShader.Get(), nullptr, 0);
	context->Dispatch(mMaxCount, 1, 1);

	context->CSSetShader(nullptr,nullptr,0);
	ID3D11UnorderedAccessView* uav[2] = { nullptr,nullptr };
	context->CSSetUnorderedAccessViews(0, 2, uav, nullptr);

}

void TitleTextureParticle::Render(ID3D11DeviceContext* context)
{
	//if (!mStartFlag)return;
	//mShader->Activate(context);

	//u_int stride = sizeof(ParticleData);
	//u_int offset = 0;

	//context->IASetVertexBuffers(0, 1, buffer.GetAddressOf(), &stride, &offset);
	//context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

	//context->Draw(mMaxCount, 0);
	//mShader->Deactivate(context);

}
