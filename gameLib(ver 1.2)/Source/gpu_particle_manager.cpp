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
		desc.FillMode = D3D11_FILL_SOLID; //D3D11_FILL_WIREFRAME, D3D11_FILL_SOLID
		desc.CullMode = D3D11_CULL_NONE; //D3D11_CULL_NONE, D3D11_CULL_FRONT, D3D11_CULL_BACK   
		desc.FrontCounterClockwise = FALSE;
		desc.DepthBias = 0;
		desc.DepthBiasClamp = 0;
		desc.SlopeScaledDepthBias = 0;
		desc.DepthClipEnable = true;
		desc.ScissorEnable = FALSE;
		desc.MultisampleEnable = false;
		desc.AntialiasedLineEnable = FALSE;
		hr = device->CreateRasterizerState(&desc, mRasterizer.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	}
	//深度ステンシル
	{
		D3D11_DEPTH_STENCIL_DESC desc = {};
		desc.DepthEnable = TRUE;
		desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		desc.DepthFunc = D3D11_COMPARISON_LESS;
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
	//D3D11_INPUT_ELEMENT_DESC inputElementDesc[] =
	//{
	//	{"POSITION",0,DXGI_FORMAT_R32G32B32A32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
	//	{"LIFE",0,DXGI_FORMAT_R32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
	//	{"SCALE",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
	//	{"COLOR",0,DXGI_FORMAT_R32G32B32A32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
	//	{"VELOCITY",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
	//	{"COLORTYPE",0,DXGI_FORMAT_R32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
	//	{"ANGLE",0,DXGI_FORMAT_R32G32B32_FLOAT,0,D3D11_APPEND_ALIGNED_ELEMENT,D3D11_INPUT_PER_VERTEX_DATA,0},
	//};
	//mSSceneShader = std::make_unique<DrowShader>(device, "Data/shader/stage_scene_particle_vs.cso", "Data/shader/run_particle_gs.cso", "Data/shader/deferred_depth_stage_scene_particle_ps.cso", inputElementDesc, ARRAYSIZE(inputElementDesc));
	mSelectSceneParticle = std::make_unique<SelectSceneParticle>(device);
	mRunParticle = std::make_unique<RunParticles>(device);
	mStageSceneParticle = std::make_unique<StageSceneParticle>(device);
}

void GpuParticleManager::ClearBuffer()
{
	mRunParticle.reset();
	mTitleParticle.reset();
	mStageObjParticle.reset();
	mStageSceneParticle.reset();
}

void GpuParticleManager::CreateStageObjParticle(std::vector<std::shared_ptr<StageObj>> objs)
{
	mStageObjParticle.reset(nullptr);
	ID3D11Device* device = Framework::Instance().GetDevice().Get();
	mStageObjParticle = std::make_unique<StageObjParticle>(device);
	mStageObjParticle->SetStageData(objs);
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
		mStageObjParticle->Update(context, elapsd_time);
		mStageSceneParticle->Update(context, elapsd_time);
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
	static bool selects[3] = { false,false };
	ImGui::Begin("game particles");
	ImGui::Selectable("run particle", &selects[0]);
	ImGui::Selectable("stage scene particle", &selects[1]);
	ImGui::Selectable("stage obj particle", &selects[2]);
	ImGui::End();
	if (selects[0])
	{
		if (mRunParticle.get() != nullptr)mRunParticle->ImGuiUpdate();
	}
	if (selects[1])
	{
		mStageSceneParticle->ImGuiUpdate();
	}
	if (selects[2])
	{
		mStageObjParticle->ImGuiUpdate();
	}

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
		//mStageSceneParticle->Render(context);
		if (mRunParticle.get() != nullptr)mRunParticle->Render(context);
		//mStageObjParticle->Render(context);
		mStageSceneParticle->Render(context);
		break;
	}
	//if (drowMullti)
	//{
	//	if (mStageSceneParticle.get() != nullptr)mStageSceneParticle->Render(context, mSSceneShader.get());
	//	//if (mStageObjParticle.get() != nullptr)mStageObjParticle->Render(context);
	//	//if (mRunParticle.get() != nullptr)mRunParticle->Render(context);
	//}
	//else
	//{
		//if (mTitleTextureParticle.get() != nullptr)mTitleTextureParticle->Render(context);
		//if (mStageObjParticle.get() != nullptr)mStageObjParticle->Render(context);
		//if (mRunParticle.get() != nullptr)mRunParticle->Render(context);
	//}
}

