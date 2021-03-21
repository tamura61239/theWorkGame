#include "sky_map.h"
#include"misc.h"
#include"shader.h"
#include"texture.h"

SkyMap::SkyMap(ID3D11Device* device, const wchar_t* textureName, MAPTYPE mapType)
{
	switch (mapType)
	{
	case MAPTYPE::BOX:
		mObjData = std::make_unique<GeometricCube>(device);
		break;
	case MAPTYPE::SPHERE:
		mObjData = std::make_unique<GeometricSphere>(device, 32, 16);
		break;
	}
	mPosData = std::make_unique<Obj3D>();
	HRESULT hr = S_OK;
	////サンプラーステートの生成
	//{
	//	D3D11_SAMPLER_DESC desc;
	//	::memset(&desc, 0, sizeof(desc));
	//	desc.MipLODBias = 0.0f;
	//	desc.MaxAnisotropy = 1;
	//	desc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	//	desc.MinLOD = -FLT_MAX;
	//	desc.MaxLOD = FLT_MAX;
	//	desc.BorderColor[0] = 1.0f;
	//	desc.BorderColor[1] = 1.0f;
	//	desc.BorderColor[2] = 1.0f;
	//	desc.BorderColor[3] = 1.0f;
	//	desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	//	desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	//	desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	//	desc.Filter = D3D11_FILTER_ANISOTROPIC;

	//	HRESULT hr = device->CreateSamplerState(&desc, mSapmleState.GetAddressOf());
	//	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	//}
	//// ラスタライザステートの生成
	//{
	//	// ラスタライザステートを作成するための設定オプション
	//	D3D11_RASTERIZER_DESC desc;
	//	ZeroMemory(&desc, sizeof(D3D11_RASTERIZER_DESC));
	//	desc.FillMode = D3D11_FILL_SOLID;
	//	desc.CullMode = D3D11_CULL_BACK;
	//	desc.FrontCounterClockwise = true;
	//	desc.DepthBias = 0;
	//	desc.DepthBiasClamp = 0.0f;
	//	desc.SlopeScaledDepthBias = 0.0f;
	//	desc.DepthClipEnable = false;
	//	desc.ScissorEnable = false;
	//	desc.MultisampleEnable = false;
	//	desc.AntialiasedLineEnable = false;

	//	hr = device->CreateRasterizerState(&desc, mRasterizerState.GetAddressOf());
	//	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	//}
	//// 深度ステンシルステートの設定
	//{
	//	D3D11_DEPTH_STENCIL_DESC desc;
	//	::memset(&desc, 0, sizeof(desc));
	//	desc.DepthEnable = true;
	//	desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	//	desc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	//	hr = device->CreateDepthStencilState(&desc, mDepthStencilState.GetAddressOf());
	//	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	//}
	// ラスタライザステートの生成
	mRasterizer = std::make_unique<RasterizerState>(device, D3D11_FILL_SOLID, D3D11_CULL_BACK, true, false, false, false, false);
	mSampler = std::make_unique<SamplerState>(device, D3D11_FILTER_ANISOTROPIC, D3D11_TEXTURE_ADDRESS_WRAP);
	//サンプラーステートの生成
	//定数バッファ作成
	{
		mCbSceneBuffer = std::make_unique<ConstantBuffer<CbScene>>(device);
		mCbObjBuffer = std::make_unique<ConstantBuffer<Cb>>(device);
		mCbBeforeObjBuffer = std::make_unique<ConstantBuffer<FLOAT4X4>>(device);
	}
	D3D11_INPUT_ELEMENT_DESC input_element_desc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	//hr = CreateVSFromCso(device, "Data/shader/sky_map_vs.cso", mVSShader.GetAddressOf(), mInput.GetAddressOf(), input_element_desc, ARRAYSIZE(input_element_desc));
	//_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	//hr = CreatePSFromCso(device, "Data/shader/sky_map_ps.cso", mPSShader.GetAddressOf());
	//_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	mShader = std::make_unique<DrowShader>(device, "Data/shader/sky_map_vs.cso", "", "Data/shader/sky_map_ps.cso", input_element_desc, ARRAYSIZE(input_element_desc));
	LoadTextureFromFile(device, textureName, mSRV.GetAddressOf());
}

void SkyMap::Render(ID3D11DeviceContext* context, const FLOAT4X4& view, const FLOAT4X4& projection, const VECTOR4F& color)
{
	mCbSceneBuffer->data.view = view;
	mCbSceneBuffer->data.projection = projection;
	mCbObjBuffer->data.color = color * mPosData->GetColor();
	mCbObjBuffer->data.world = mPosData->GetWorld();

	mCbSceneBuffer->Activate(context, 0, true, true);
	mCbObjBuffer->Activate(context, 1, true, true);
	ID3D11SamplerState* sampler = nullptr;
	context->PSGetSamplers(0, 1, &sampler);
	mSampler->Activate(context, 0, false, true);
	//context->PSSetSamplers(0, 1, mSapmleState.GetAddressOf());
	context->PSSetShaderResources(0, 1, mSRV.GetAddressOf());

	mShader->Activate(context);
	mRasterizer->Activate(context);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	UINT stride = sizeof(GeometricPrimitive::Vertex);
	UINT offset = 0;
	context->IASetVertexBuffers(0, 1, mObjData->GetVertexBuffer().GetAddressOf(), &stride, &offset);
	context->IASetIndexBuffer(mObjData->GetIndexBuffer().Get(), DXGI_FORMAT_R32_UINT, 0);

	context->DrawIndexed(mObjData->GetIndexNum(), 0, 0);
	ID3D11ShaderResourceView* srv = nullptr;
	context->PSSetShaderResources(0, 1, &srv);
	mShader->Deactivate(context);
	mCbSceneBuffer->DeActivate(context);
	mCbObjBuffer->DeActivate(context);
	stride = 0;
	ID3D11Buffer* vertexBuffer = nullptr;
	context->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	context->IASetIndexBuffer(nullptr, DXGI_FORMAT_R32_UINT, 0);
	mRasterizer->DeActivate(context);
	mSampler->DeActivate(context);
	context->PSSetSamplers(0, 1, &sampler);
}

void SkyMap::Render(ID3D11DeviceContext* context, DrowShader* shader, const FLOAT4X4& view, const FLOAT4X4& projection, const VECTOR4F& color)
{
	mCbSceneBuffer->data.view = view;
	mCbSceneBuffer->data.projection = projection;
	mCbObjBuffer->data.color = color * mPosData->GetColor();
	mCbObjBuffer->data.world = mPosData->GetWorld();

	mCbSceneBuffer->Activate(context, 0, true, true);
	mCbObjBuffer->Activate(context, 1, true, true);
	mCbBeforeObjBuffer->Activate(context, 2, true, true);

	//context->PSSetSamplers(0, 1, mSapmleState.GetAddressOf());
	context->PSSetShaderResources(0, 1, mSRV.GetAddressOf());

	shader->Activate(context);
	mRasterizer->Activate(context);

	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	UINT stride = sizeof(GeometricPrimitive::Vertex);
	UINT offset = 0;
	context->IASetVertexBuffers(0, 1, mObjData->GetVertexBuffer().GetAddressOf(), &stride, &offset);
	context->IASetIndexBuffer(mObjData->GetIndexBuffer().Get(), DXGI_FORMAT_R32_UINT, 0);

	context->DrawIndexed(mObjData->GetIndexNum(), 0, 0);

	shader->Deactivate(context);
	mCbSceneBuffer->DeActivate(context);
	mCbObjBuffer->DeActivate(context);
	mCbBeforeObjBuffer->DeActivate(context);

	stride = 0;
	context->IASetVertexBuffers(0, 1, nullptr, &stride, &offset);
	context->IASetIndexBuffer(nullptr, DXGI_FORMAT_R32_UINT, 0);
	mRasterizer->DeActivate(context);
}
