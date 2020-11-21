#include "title_texture_particle.h"
#include"texture.h"
#include"misc.h"
#include"shader.h"
#include"framework.h"
#include"camera_manager.h"
#ifdef USE_IMGUI
#include<imgui.h>
#endif


TitleTextureParticle::TitleTextureParticle(ID3D11Device* device) :mSceneDrowFlag(false), mParticleFlag(false), mMaxParticle(1920 * 1080)
{
	mRender = std::make_unique<Sprite>(device);
	mArrangementSceneTexture= std::make_unique<FrameBuffer>(device, 1920, 1080, true, 1, DXGI_FORMAT_R16G16B16A16_FLOAT);
	mMaxParticle /= 10;
	HRESULT hr;
	Microsoft::WRL::ComPtr<ID3D11Buffer>buffer;
	//Buffer
	{
		std::vector<Particle>particles;
		particles.resize(mMaxParticle);
		D3D11_BUFFER_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		//パーティクルデータバッファ
		desc.ByteWidth = sizeof(Particle) * mMaxParticle;
		desc.Usage = D3D11_USAGE_DEFAULT;//ステージの入出力はOK。GPUの入出力OK。
		desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
		desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED; // 構造化バッファ
		desc.StructureByteStride = sizeof(Particle);
		D3D11_SUBRESOURCE_DATA data;
		ZeroMemory(&data, sizeof(data));
		data.pSysMem = &particles[0];
		hr = device->CreateBuffer(&desc, &data, buffer.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		//描画用バッファ
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER | D3D11_BIND_UNORDERED_ACCESS;
		desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS;
		desc.ByteWidth = sizeof(RenderParticle) * mMaxParticle;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.CPUAccessFlags = 0;
		desc.StructureByteStride = 0;
		std::vector<RenderParticle>renderParticles;
		renderParticles.resize(mMaxParticle);
		data.pSysMem = &renderParticles[0];
		hr = device->CreateBuffer(&desc, &data, mRenderBuffer.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		//定数バッファ
		desc.ByteWidth = sizeof(CbUpdate);
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = 0;
		desc.StructureByteStride = 0;
		hr = device->CreateBuffer(&desc, nullptr, mCbBuffer.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		desc.ByteWidth = sizeof(CbCamera);

		hr = device->CreateBuffer(&desc, nullptr, mCbCameraBuffer.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));


	}
	//UAV
	{
		D3D11_UNORDERED_ACCESS_VIEW_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.Buffer.NumElements =  mMaxParticle;
		desc.Format = DXGI_FORMAT_UNKNOWN;
		desc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;

		hr = device->CreateUnorderedAccessView(buffer.Get(), &desc, mParticleUAV.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		desc.Format = DXGI_FORMAT_R32_TYPELESS;
		desc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
		desc.Buffer.FirstElement = 0;
		desc.Buffer.Flags = D3D11_BUFFEREX_SRV_FLAG_RAW;

		desc.Buffer.NumElements = sizeof(RenderParticle) * mMaxParticle / 4;
		hr = device->CreateUnorderedAccessView(mRenderBuffer.Get(), &desc, mRenderUAV.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	}
	create_cs_from_cso(device, "Data/shader/title_texture_particle_create_cs.cso", mCreateCSShader.GetAddressOf());
	create_cs_from_cso(device, "Data/shader/title_texture_particle_cs.cso", mCSShader.GetAddressOf());
	D3D11_INPUT_ELEMENT_DESC inputElementDesc[] =
	{
		{"POSITION",0,DXGI_FORMAT_R32G32B32A32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
		{"ANGLE",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
		{"COLOR",0,DXGI_FORMAT_R32G32B32A32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
		{"VELOCITY",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
		{"SCALE",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
	};
	//mShader = std::make_unique<DrowShader>(device, "Data/shader/particle_render_vs.cso", "", "Data/shader/particle_render_point_ps.cso", inputElementDesc, ARRAYSIZE(inputElementDesc));
	mShader = std::make_unique<DrowShader>(device, "Data/shader/particle_render_vs.cso", "Data/shader/particle_render_billboard_gs.cso", "Data/shader/particle_render_text_ps.cso", inputElementDesc, ARRAYSIZE(inputElementDesc));
	//mShader = std::make_unique<DrowShader>(device, "Data/shader/particle_render_vs.cso", "Data/shader/particle_render_cube_mesh_gs.cso", "Data/shader/particle_render_ps.cso", inputElementDesc, ARRAYSIZE(inputElementDesc));
	//hr = make_dummy_texture(device, mParticleSRV.GetAddressOf());
	hr = load_texture_from_file(device,  L"Data/image/○.png", mParticleSRV.GetAddressOf());
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	blend = std::make_unique<blend_state>(device, BLEND_MODE::ALPHA);
	mEditorData.scale = 0.00015f;
	mEditorData.ndcZ = 0.1f;
	mEditorData.spiralRatio = VECTOR2F(1, 1);
	
}

void TitleTextureParticle::ImGuiUpdate()
{
#ifdef USE_IMGUI
	ImGui::Begin("title texture particle");
	ImGui::Image(mArrangementSceneTexture->GetRenderTargetShaderResourceView().Get(), ImVec2(1920 / 5, 1080 / 5) );
	ImGui::Checkbox("create", &mSceneDrowFlag);
	ImGui::Checkbox("move", &mParticleFlag);
	ImGui::InputFloat("ndc z", &mEditorData.ndcZ, 0.001f);
	ImGui::InputFloat("speed", &mEditorData.speed, 0.01f);
	ImGui::InputFloat("scale", &mEditorData.scale, 0.00001f,0,"%f");
	float* spiralRatio[2] = { &mEditorData.spiralRatio.x,&mEditorData.spiralRatio.y };
	ImGui::DragFloat2("spiralRatio", *spiralRatio, 1);
	ImGui::End();
#endif
}

void TitleTextureParticle::LoadTexture(ID3D11Device* device,std::wstring name, const VECTOR2F& leftTop, const VECTOR2F& size, const VECTOR2F& uv, const VECTOR2F& uvSize)
{
	mTextures.emplace_back();
	auto& textute = mTextures.back();
	HRESULT hr = load_texture_from_file(device, name.c_str(), textute.mSRV.GetAddressOf());
	textute.data.mLeftTop = leftTop;
	textute.data.mSize = size;
	textute.data.mUVLeftTop = uv;
	textute.data.mUVSize = uvSize;
	textute.data.mTextureName = name;
}

void TitleTextureParticle::Update(float elapsdTime, ID3D11DeviceContext* context)
{
	if (mSceneDrowFlag)
	{
		mArrangementSceneTexture->Clear(context);
		mArrangementSceneTexture->Activate(context);
		blend->activate(context);
		for (auto& textute : mTextures)
		{
			mRender->Render(context, textute.mSRV.Get(), textute.data.mLeftTop, textute.data.mSize, textute.data.mUVLeftTop, textute.data.mUVSize, 0);
		}
		blend->deactivate(context);
		mArrangementSceneTexture->Deactivate(context);
		mSceneDrowFlag = false;
		mParticleFlag = true;
		//生成
		context->CSSetShader(mCreateCSShader.Get(), nullptr, 0);
		context->CSSetShaderResources(0, 1, mArrangementSceneTexture->GetRenderTargetShaderResourceView().GetAddressOf());
		context->CSSetUnorderedAccessViews(0, 1, mParticleUAV.GetAddressOf(), nullptr);

		CbCamera cbCamera;
		cbCamera.ndcZ = mEditorData.ndcZ;
		const auto& camera = pCameraManager->GetCamera();
		DirectX::XMMATRIX W = DirectX::XMMatrixIdentity();//移動成分はいらないので単位行列を入れておく。
		FLOAT4X4 view = camera->GetView();
		DirectX::XMStoreFloat4x4(&cbCamera.inverseVP, DirectX::XMMatrixInverse(nullptr, DirectX::XMLoadFloat4x4(&view) * DirectX::XMLoadFloat4x4(&camera->GetProjection())));
		context->CSSetConstantBuffers(0, 1, mCbCameraBuffer.GetAddressOf());
		context->UpdateSubresource(mCbCameraBuffer.Get(), 0, 0, &cbCamera, 0, 0);

		context->Dispatch(mMaxParticle, 1, 1);

		CbUpdate cbUpdate;
		cbUpdate.elapsdTime = 0;
		cbUpdate.scale = mEditorData.scale;
		cbUpdate.speed = mEditorData.speed;
		cbUpdate.spiralRatio = mEditorData.spiralRatio;

		context->CSSetConstantBuffers(1, 1, mCbBuffer.GetAddressOf());
		context->UpdateSubresource(mCbBuffer.Get(), 0, 0, &cbUpdate, 0, 0);

		context->CSSetShader(mCSShader.Get(), nullptr, 0);
		//context->CSSetUnorderedAccessViews(0, 1, mParticleUAV.GetAddressOf(), nullptr);
		context->CSSetUnorderedAccessViews(2, 1, mRenderUAV.GetAddressOf(), nullptr);
		context->Dispatch(mMaxParticle / 100, 1, 1);
		ID3D11UnorderedAccessView* uav = nullptr;
		context->CSSetShader(nullptr, nullptr, 0);
		context->CSSetUnorderedAccessViews(0, 1, &uav, nullptr);
		context->CSSetUnorderedAccessViews(2, 1, &uav, nullptr);
		return;
	}
	if (!mParticleFlag)return;
	CbUpdate cbUpdate;
	cbUpdate.elapsdTime = elapsdTime;
	cbUpdate.scale = mEditorData.scale;
	cbUpdate.speed = mEditorData.speed;
	context->CSSetConstantBuffers(1, 1, mCbBuffer.GetAddressOf());
	context->UpdateSubresource(mCbBuffer.Get(), 0, 0, &cbUpdate, 0, 0);

	context->CSSetShader(mCSShader.Get(), nullptr, 0);
	context->CSSetUnorderedAccessViews(0, 1, mParticleUAV.GetAddressOf(), nullptr);
	context->CSSetUnorderedAccessViews(2, 1, mRenderUAV.GetAddressOf(), nullptr);
	context->Dispatch(mMaxParticle/100, 1, 1);
	ID3D11UnorderedAccessView* uav = nullptr;
	context->CSSetShader(nullptr, nullptr, 0);
	context->CSSetUnorderedAccessViews(0, 1, &uav, nullptr);
	context->CSSetUnorderedAccessViews(2, 1, &uav, nullptr);

}

void TitleTextureParticle::Render(ID3D11DeviceContext* context)
{
	//if (!mParticleFlag && !mSceneDrowFlag)return;
	mShader->Activate(context);

	u_int stride = sizeof(RenderParticle);
	u_int offset = 0;
	context->PSSetShaderResources(0, 1, mParticleSRV.GetAddressOf());
	context->IASetVertexBuffers(0, 1, mRenderBuffer.GetAddressOf(), &stride, &offset);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

	context->Draw(mMaxParticle, 0);
	mShader->Deactivate(context);
	ID3D11ShaderResourceView* srv = nullptr;
	context->PSSetShaderResources(0, 1, &srv);

}

