#include "gpu_particle_manager.h"
#include"framework.h"
#include"misc.h"


void GpuParticleManager::CreateBuffer(ID3D11Device* device)
{
	HRESULT hr = S_OK;
	//定数バッファ作成
	{
		D3D11_BUFFER_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.ByteWidth = sizeof(CbScene);
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = 0;
		desc.StructureByteStride = 0;
		hr = device->CreateBuffer(&desc, nullptr, mCbScene.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}
	//ラスタライザーステート
	{
		D3D11_RASTERIZER_DESC desc = {};
		ZeroMemory(&desc, sizeof(D3D11_RASTERIZER_DESC));
		desc.FillMode = D3D11_FILL_SOLID; //D3D11_FILL_WIREFRAME, D3D11_FILL_SOLID
		desc.CullMode = D3D11_CULL_NONE; //D3D11_CULL_NONE, D3D11_CULL_FRONT, D3D11_CULL_BACK   
		desc.FrontCounterClockwise = FALSE;
		desc.DepthBias = 0;
		desc.DepthBiasClamp = 0;
		desc.SlopeScaledDepthBias = 0;
		desc.DepthClipEnable = true;
		desc.ScissorEnable = FALSE;
		desc.MultisampleEnable = true;
		desc.AntialiasedLineEnable = FALSE;
		hr = device->CreateRasterizerState(&desc, mRasterizer.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	}
	//深度ステンシル
	{
		D3D11_DEPTH_STENCIL_DESC desc = {};
		ZeroMemory(&desc, sizeof(desc));
		desc.DepthEnable = TRUE;
		desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		desc.DepthFunc = D3D11_COMPARISON_NOT_EQUAL;
		desc.StencilEnable = FALSE;
		desc.StencilReadMask = 0xFF;
		desc.StencilWriteMask = 0xFF;
		desc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		desc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
		desc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		desc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
		desc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		desc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
		desc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		desc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
		hr = device->CreateDepthStencilState(&desc, mDepth.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	}
	D3D11_INPUT_ELEMENT_DESC inputElementDesc[] =
	{
		{"POSITION",0,DXGI_FORMAT_R32G32B32A32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
		{"ANGLE",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
		{"COLOR",0,DXGI_FORMAT_R32G32B32A32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
		{"VELOCITY",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
		{"SCALE",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
	};
	mSSceneShader = std::make_unique<DrowShader>(device, "Data/shader/particle_motion_data_render_vs.cso", "Data/shader/particle_motion_data_render_gs.cso", "Data/shader/particle_motion_data_render_ps.cso", inputElementDesc, ARRAYSIZE(inputElementDesc));

}

void GpuParticleManager::CreateTitleBuffer(ID3D11Device* device)
{
	CreateBuffer(device);
	mTitleParticle = std::make_unique<TitleParticle>(device);
	mTitleTextureParticle = std::make_unique<TitleTextureParticle>(device);
}

void GpuParticleManager::CreateGameBuffer(ID3D11Device* device)
{
	mState = 0;
	CreateBuffer(device);
	mSelectSceneParticle = std::make_unique<SelectSceneParticle>(device);
	mRunParticle = std::make_unique<RunParticles>(device);
	mStageSceneParticle = std::make_unique<StageSceneParticle>(device);
}

void GpuParticleManager::CreateResultBuffer(ID3D11Device* device)
{
	CreateBuffer(device);

#if (RESULT_TYPE==0)

	mFireworksParticle = std::make_unique<FireworksParticle>(device);
#else
	mStageSceneParticle = std::make_unique<StageSceneParticle>(device);
#endif
}

void GpuParticleManager::ClearBuffer()
{
	mRunParticle.reset();
	mTitleParticle.reset();
	mStageSceneParticle.reset();
	mFireworksParticle.reset();
}


void GpuParticleManager::Update(float elapsd_time)
{
	ID3D11DeviceContext* context = Framework::Instance().GetDeviceContext().Get();
	switch (mState)
	{
	case TITLE:
		mTitleParticle->Update(elapsd_time, context);
		mTitleTextureParticle->Update(elapsd_time, context);
		break;
	case SELECT:
		mSelectSceneParticle->Update(elapsd_time, context);
		break;
	case GAME:
		if (mRunParticle.get() != nullptr)mRunParticle->Update(context, elapsd_time);
		mStageSceneParticle->Update(context, elapsd_time);
		break;
	case RESULT:
#if (RESULT_TYPE==0)
		mFireworksParticle->Update(elapsd_time, context);
#else
		mStageSceneParticle->Update(context, elapsd_time);
#endif
		break;
	}
}


void GpuParticleManager::ImGuiUpdate()
{
	switch (mState)
	{
	case TITLE:
		TitleImGui();
		break;
	case SELECT:
		SelectImGui();
		break;
	case GAME:
		GameImGui();
		break;
	case RESULT:
		ResultImGui();
		break;
	}
}
void GpuParticleManager::TitleImGui()
{
#ifdef USE_IMGUI
	static bool selects[2] = { false,false };
	ImGui::Begin("title particles");
	ImGui::Selectable("title scene particle", &selects[0]);
	ImGui::Selectable("title text particle", &selects[1]);
	ImGui::End();
	if (selects[0])
	{
		mTitleParticle->ImGuiUpdate();
	}
	if (selects[1])
	{
		mTitleTextureParticle->ImGuiUpdate();
	}

#endif

}

void GpuParticleManager::SelectImGui()
{
#ifdef USE_IMGUI
	static bool selects[2] = { false,false };
	ImGui::Begin("select particles");
	ImGui::Selectable("select scene particle", &selects[0]);
	ImGui::End();
	if (selects[0])
	{
		mSelectSceneParticle->ImGuiUpdate();
	}

#endif

}

void GpuParticleManager::GameImGui()
{
#ifdef USE_IMGUI
	static bool selects[2] = { false,false };
	ImGui::Begin("game particles");
	ImGui::Selectable("run particle", &selects[0]);
	ImGui::Selectable("stage scene particle", &selects[1]);
	ImGui::End();
	if (selects[0])
	{
		if (mRunParticle.get() != nullptr)mRunParticle->ImGuiUpdate();
	}
	if (selects[1])
	{
		mStageSceneParticle->ImGuiUpdate();
}

#endif

}

void GpuParticleManager::ResultImGui()
{
#if (RESULT_TYPE==0)
	mFireworksParticle->ImGuiUpdate();
#else
#endif
}

void GpuParticleManager::Render(ID3D11DeviceContext* context, const FLOAT4X4& view, const FLOAT4X4& projection, bool drowMullti)
{
	context->OMSetDepthStencilState(mDepth.Get(), 0);
	context->RSSetState(mRasterizer.Get());
	context->VSSetConstantBuffers(0, 1, mCbScene.GetAddressOf());
	context->GSSetConstantBuffers(0, 1, mCbScene.GetAddressOf());
	context->PSSetConstantBuffers(0, 1, mCbScene.GetAddressOf());

	CbScene cbScene;
	cbScene.view = view;
	cbScene.projection = projection;
	context->UpdateSubresource(mCbScene.Get(), 0, 0, &cbScene, 0, 0);
	switch (mState)
	{
	case TITLE:
		mTitleParticle->Render(context);
		mTitleTextureParticle->Render(context);
		break;
	case SELECT:
		mSelectSceneParticle->Render(context);
		break;
	case GAME:
		//if (mRunParticle.get() != nullptr)mRunParticle->Render(context);
		mStageSceneParticle->Render(context);
		break;
	case RESULT:
		mFireworksParticle->Render(context);
		break;
	}
}

void GpuParticleManager::VelocityRender(ID3D11DeviceContext* context, const FLOAT4X4& view, const FLOAT4X4& projection)
{
	context->OMSetDepthStencilState(mDepth.Get(), 0);
	context->RSSetState(mRasterizer.Get());
	context->VSSetConstantBuffers(0, 1, mCbScene.GetAddressOf());
	context->GSSetConstantBuffers(0, 1, mCbScene.GetAddressOf());
	context->PSSetConstantBuffers(0, 1, mCbScene.GetAddressOf());

	CbScene cbScene;
	cbScene.view = view;
	cbScene.projection = projection;
	context->UpdateSubresource(mCbScene.Get(), 0, 0, &cbScene, 0, 0);
	switch (mState)
	{
	case SELECT:
		mSelectSceneParticle->Render(context);
		break;
	case GAME:
		//if (mRunParticle.get() != nullptr)mRunParticle->Render(context,mSSceneShader.get());
		mStageSceneParticle->Render(context, mSSceneShader.get());
		break;
	case RESULT:
#if (RESULT_TYPE==0)
		mFireworksParticle->Render(context,mSSceneShader.get());
#else
		mStageSceneParticle->Render(context);
#endif
		break;
	}

}

