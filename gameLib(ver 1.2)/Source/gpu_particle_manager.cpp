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
		desc.CullMode = D3D11_CULL_BACK; //D3D11_CULL_NONE, D3D11_CULL_FRONT, D3D11_CULL_BACK   
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
	mRunParticle = std::make_unique<RunParticles>(device);
}

void GpuParticleManager::ClearBuffer()
{
	mRunParticle.reset();
}

void GpuParticleManager::CreateStageObjParticle(std::vector<std::shared_ptr<StageObj>> objs)
{
	mStageObjParticle.reset(nullptr);
	ID3D11Device* device = Framework::Instance().GetDevice().Get();
	mStageObjParticle = std::make_unique<StageObjParticle>(device);
	mStageObjParticle->CreateBuffer(device, objs);
}

void GpuParticleManager::Update(float elapsd_time, float colorType, const VECTOR3F& velocity, const VECTOR3F& position, const bool groundFlag)
{
	ID3D11DeviceContext* context = Framework::Instance().GetDeviceContext().Get();
	if (mStageObjParticle.get() != nullptr)mStageObjParticle->Update(context, elapsd_time, colorType);
	if (mRunParticle.get() != nullptr)mRunParticle->Update(context, elapsd_time, velocity, groundFlag, position);
}

void GpuParticleManager::Render(ID3D11DeviceContext* context, const FLOAT4X4& view, const FLOAT4X4& projection)
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
	if (mStageObjParticle.get() != nullptr)mStageObjParticle->Render(context);
	if (mRunParticle.get() != nullptr)mRunParticle->Render(context);
}
