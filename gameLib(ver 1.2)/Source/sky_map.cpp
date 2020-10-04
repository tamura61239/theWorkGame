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
		mObjData = std::make_unique<GeometricSphere>(device,32,16);
		break;
	}
	mPosData = std::make_unique<Obj3D>();
	HRESULT hr = S_OK;
	//サンプラーステートの生成
	{
		D3D11_SAMPLER_DESC desc;
		::memset(&desc, 0, sizeof(desc));
		desc.MipLODBias = 0.0f;
		desc.MaxAnisotropy = 1;
		desc.ComparisonFunc = D3D11_COMPARISON_NEVER;
		desc.MinLOD = -FLT_MAX;
		desc.MaxLOD = FLT_MAX;
		desc.BorderColor[0] = 1.0f;
		desc.BorderColor[1] = 1.0f;
		desc.BorderColor[2] = 1.0f;
		desc.BorderColor[3] = 1.0f;
		desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		desc.Filter = D3D11_FILTER_ANISOTROPIC;

		HRESULT hr = device->CreateSamplerState(&desc, mSapmleState.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	}
	// ラスタライザステートの生成
	{
		// ラスタライザステートを作成するための設定オプション
		D3D11_RASTERIZER_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_RASTERIZER_DESC));
		desc.FillMode = D3D11_FILL_SOLID;
		desc.CullMode = D3D11_CULL_BACK;
		desc.FrontCounterClockwise = true;
		desc.DepthBias = 0;
		desc.DepthBiasClamp = 0.0f;
		desc.SlopeScaledDepthBias = 0.0f;
		desc.DepthClipEnable = false;
		desc.ScissorEnable = false;
		desc.MultisampleEnable = false;
		desc.AntialiasedLineEnable = false;

		hr = device->CreateRasterizerState(&desc, mRasterizerState.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}
	// 深度ステンシルステートの設定
	{
		D3D11_DEPTH_STENCIL_DESC desc;
		::memset(&desc, 0, sizeof(desc));
		desc.DepthEnable = true;
		desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		desc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
		hr = device->CreateDepthStencilState(&desc, mDepthStencilState.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}
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

		hr = device->CreateBuffer(&desc, nullptr, mCbSceneBuffer.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

		desc.ByteWidth = sizeof(Cb);
		hr = device->CreateBuffer(&desc, nullptr, mCbObjBuffer.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

		desc.ByteWidth = sizeof(FLOAT4X4);
		hr = device->CreateBuffer(&desc, nullptr, mCbBeforeObjBuffer.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	}
	D3D11_INPUT_ELEMENT_DESC input_element_desc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	//hr = create_vs_from_cso(device, "Data/shader/sky_map_vs.cso", mVSShader.GetAddressOf(), mInput.GetAddressOf(), input_element_desc, ARRAYSIZE(input_element_desc));
	//_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	//hr = create_ps_from_cso(device, "Data/shader/sky_map_ps.cso", mPSShader.GetAddressOf());
	//_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	mShader = std::make_unique<DrowShader>(device, "Data/shader/sky_map_vs.cso", "", "Data/shader/sky_map_ps.cso", input_element_desc, ARRAYSIZE(input_element_desc));
	load_texture_from_file(device, textureName, mSRV.GetAddressOf());
}

void SkyMap::Render(ID3D11DeviceContext* context, const FLOAT4X4& view, const FLOAT4X4& projection, const VECTOR4F& color)
{
	CbScene cbScene;
	cbScene.view = view;
	cbScene.projection = projection;
	Cb cb;
	cb.color = color;
	cb.world = mPosData->GetWorld();
	ID3D11Buffer* buffer[] =
	{
		mCbSceneBuffer.Get(),
		mCbObjBuffer.Get(),
	};
	context->PSSetConstantBuffers(0, 2, buffer);
	context->VSSetConstantBuffers(0, 2, buffer);

	context->UpdateSubresource(mCbSceneBuffer.Get(), 0, 0, &cbScene, 0, 0);
	context->UpdateSubresource(mCbObjBuffer.Get(), 0, 0, &cb, 0, 0);

	context->PSSetSamplers(0, 1, mSapmleState.GetAddressOf());
	context->PSSetShaderResources(0, 1, mSRV.GetAddressOf());

	mShader->Activate(context);
	context->RSSetState(mRasterizerState.Get());

	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	UINT stride = sizeof(GeometricPrimitive::Vertex);
	UINT offset = 0;
	context->IASetVertexBuffers(0, 1, mObjData->GetVertexBuffer().GetAddressOf(), &stride, &offset);
	context->IASetIndexBuffer(mObjData->GetIndexBuffer().Get(), DXGI_FORMAT_R32_UINT, 0);

	context->DrawIndexed(mObjData->GetIndexNum(), 0, 0);
	ID3D11ShaderResourceView* srv = nullptr;
	context->PSSetShaderResources(0, 1, &srv);
	mShader->Deactivate(context);
}

void SkyMap::Render(ID3D11DeviceContext* context, DrowShader* shader, const FLOAT4X4& view, const FLOAT4X4& projection, const VECTOR4F& color)
{
	CbScene cbScene;
	cbScene.view = view;
	cbScene.projection = projection;
	Cb cb;
	cb.color = color;
	cb.world = mPosData->GetWorld();
	ID3D11Buffer* buffer[] =
	{
		mCbSceneBuffer.Get(),
		mCbObjBuffer.Get(),
		mCbBeforeObjBuffer.Get(),
	};
	context->PSSetConstantBuffers(0, 3, buffer);
	context->VSSetConstantBuffers(0, 3, buffer);

	context->UpdateSubresource(mCbSceneBuffer.Get(), 0, 0, &cbScene, 0, 0);
	context->UpdateSubresource(mCbObjBuffer.Get(), 0, 0, &cb, 0, 0);
	context->UpdateSubresource(mCbBeforeObjBuffer.Get(), 0, 0, &beforeWorld, 0, 0);

	context->PSSetSamplers(0, 1, mSapmleState.GetAddressOf());
	context->PSSetShaderResources(0, 1, mSRV.GetAddressOf());

	shader->Activate(context);
	context->RSSetState(mRasterizerState.Get());

	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	UINT stride = sizeof(GeometricPrimitive::Vertex);
	UINT offset = 0;
	context->IASetVertexBuffers(0, 1, mObjData->GetVertexBuffer().GetAddressOf(), &stride, &offset);
	context->IASetIndexBuffer(mObjData->GetIndexBuffer().Get(), DXGI_FORMAT_R32_UINT, 0);

	context->DrawIndexed(mObjData->GetIndexNum(), 0, 0);

	shader->Deactivate(context);

}
