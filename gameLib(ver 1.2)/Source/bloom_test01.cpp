#include "bloom_test01.h"
#include<WICTextureLoader.h>
#include "misc.h"
#include"screen_size.h"
#include"texture.h"
#include"shader.h"
#include"framework.h"

BlommTest01::BlommTest01(ID3D11Device* device, const wchar_t* fileName)
{
	HRESULT hr;
	Vertex vertices[] =
	{
		{VECTOR3F(-0.5f,0.5f,0),VECTOR4F(1.0f,1.0f,1.0f,1.0f),VECTOR2F(0.0f,0.0f)},
		{VECTOR3F(0.5f,0.5f,0),VECTOR4F(1.0f,1.0f,1.0f,1.0f),VECTOR2F(1.0f,0.0f)},
		{VECTOR3F(-0.5f,-0.5f,0),VECTOR4F(1.0f,1.0f,1.0f,1.0f),VECTOR2F(0.0f,1.0f)},
		{VECTOR3F(0.5f,-0.5f,0),VECTOR4F(1.0f,1.0f,1.0f,1.0f),VECTOR2F(1.0f,1.0f)},
	};

	//頂点バッファ作成
	{
		D3D11_BUFFER_DESC desc = {};
		//ZeroMemory(&desc, sizeof(desc));
		desc.Usage = D3D11_USAGE_DYNAMIC;
		desc.ByteWidth = sizeof(Vertex) * 4;
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		desc.MiscFlags = 0;
		desc.StructureByteStride = 0;
		D3D11_SUBRESOURCE_DATA data;
		data.pSysMem = vertices;
		data.SysMemPitch = 0;
		data.SysMemSlicePitch = 0;
		hr = device->CreateBuffer(&desc, &data, mVSBuffer.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}
	//シェーダー
	{
		D3D11_INPUT_ELEMENT_DESC input_element_desc[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
		};

		create_vs_from_cso(device, "Data/shader/sprite_vs.cso", mVSShader.GetAddressOf(), mInput.GetAddressOf(), input_element_desc, ARRAYSIZE(input_element_desc));
		create_ps_from_cso(device, "Data/shader/sprite_ps.cso", mPSShader.GetAddressOf());

	}
	//テクスチャ
	{
		load_texture_from_file(device, fileName, mTexview.GetAddressOf(), &texture2dDesc);
	}
	//RasterizerStateの生成
	{
		D3D11_RASTERIZER_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_RASTERIZER_DESC));
		desc.FillMode = D3D11_FILL_SOLID;
		desc.CullMode = D3D11_CULL_NONE;
		desc.FrontCounterClockwise = false;
		desc.DepthBias = 0;
		desc.DepthBiasClamp = 0.0f;
		desc.SlopeScaledDepthBias = 0.0f;
		desc.DepthClipEnable = true;
		desc.ScissorEnable = false;
		desc.MultisampleEnable = true;
		desc.AntialiasedLineEnable = false;

		hr = device->CreateRasterizerState(&desc, mRasterizerState.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}

	//ShaderResourceViewの生成

	//SamplerStateの生成
	{
		D3D11_SAMPLER_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
		desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		desc.ComparisonFunc = D3D11_COMPARISON_NEVER;
		desc.MinLOD = -FLT_MAX;
		desc.MaxLOD = FLT_MAX;
		desc.MaxAnisotropy = 16;
		memcpy(desc.BorderColor, &VECTOR4F(1.0f, 1.0f, 1.0f, 1.0f), sizeof(VECTOR4F));
		hr = device->CreateSamplerState(&desc, mSamplerState.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}
	//DepthStencilStateの生成
	{
		D3D11_DEPTH_STENCIL_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.DepthEnable = false;
		desc.StencilEnable = false;
		desc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
		desc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;
		desc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		desc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
		desc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_INCR;
		desc.FrontFace.StencilFunc = D3D11_COMPARISON_GREATER_EQUAL;
		desc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		desc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
		desc.BackFace.StencilPassOp = D3D11_STENCIL_OP_INCR;
		desc.BackFace.StencilFunc = D3D11_COMPARISON_GREATER_EQUAL;
		hr = device->CreateDepthStencilState(&desc, mDepthStencilState.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}

	//空のテクスチャ作成
	DXGI_FORMAT format = DXGI_FORMAT_R16G16B16A16_FLOAT;
	UINT width = static_cast<UINT>(SCREEN_WIDTH);
	UINT height = static_cast<UINT>(SCREEN_HEIGHT);
	for (int i = 0;i < 5;i++)
	{
		width /= 4;
		height /= 4;
		Microsoft::WRL::ComPtr<ID3D11Texture2D>texture2d;
		{
			D3D11_TEXTURE2D_DESC texture2d_desc;
			ZeroMemory(&texture2d_desc, sizeof(texture2d_desc));
			texture2d_desc.Width = static_cast<UINT>(SCREEN_WIDTH);
			texture2d_desc.Height = static_cast<UINT>(SCREEN_HEIGHT);
			texture2d_desc.MipLevels = 1;
			texture2d_desc.ArraySize = 1;
			texture2d_desc.Format = format;
			texture2d_desc.SampleDesc.Count = 1;
			texture2d_desc.Usage = D3D11_USAGE_DEFAULT;
			texture2d_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			texture2d_desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
			hr = device->CreateTexture2D(&texture2d_desc, nullptr, texture2d.GetAddressOf());
		}
		//レンダーターゲットビュー作成
		{
			D3D11_RENDER_TARGET_VIEW_DESC rtvd;
			ZeroMemory(&rtvd, sizeof(rtvd));
			rtvd.Format = format;
			rtvd.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
			rtvd.Texture2D.MipSlice = 0;
			hr = device->CreateRenderTargetView(texture2d.Get(), &rtvd, mRenderTargetView[i].GetAddressOf());

		}
		//	シェーダーリソースビュー作成
		{
			//D3D11_TEXTURE2D_DESC texture2d_desc;
			//texture2d->GetDesc(&texture2d_desc);
			//texture2d_desc.Width = width;
			//texture2d_desc.Height = height;
			//hr = device->CreateTexture2D(&texture2d_desc, nullptr, texture2d.GetAddressOf());
			D3D11_SHADER_RESOURCE_VIEW_DESC srvd;
			ZeroMemory(&srvd, sizeof(srvd));
			srvd.Format = format;
			srvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
			srvd.Texture2D.MostDetailedMip = 0;
			srvd.Texture2D.MipLevels = 1;
			hr = device->CreateShaderResourceView(texture2d.Get(), &srvd, mShaderResourceView[i].GetAddressOf());
		}

	}

}


void BlommTest01::Render(ID3D11DeviceContext* context, const VECTOR2F& position, const VECTOR2F& size,
	const VECTOR2F& texPosition, const VECTOR2F& texSize, float angle, const VECTOR4F& color)
{
	//UINT pstrides = sizeof(Vertex);
	//UINT poff = 0;
	//HRESULT hr;
	////D3D11_MAP map = D3D11_MAP_WRITE_DISCARD;
	//Vertex vertices[] =
	//{
	//	{VECTOR3F(position.x,position.y,0),color,VECTOR2F(texPosition.x,texPosition.y)},
	//	{VECTOR3F(position.x + size.x,position.y,0),color,VECTOR2F(texPosition.x + texSize.x,texPosition.y)},
	//	{VECTOR3F(position.x,position.y + size.y,0),color,VECTOR2F(texPosition.x,texPosition.y + texSize.y)},
	//	{VECTOR3F(position.x + size.x,position.y + size.y,0),color,VECTOR2F(texPosition.x + texSize.x,texPosition.y + texSize.y)},
	//};
	//float texX = position.x + size.x / 2.0f;
	//float texY = position.y + size.y / 2.0f;
	////	回転軸の変更(短形中央へ)
	//for (int i = 0; i < 4; i++) {
	//	vertices[i].position.x -= texX;
	//	vertices[i].position.y -= texY;
	//}
	////    回転
	//float degree = DirectX::XMConvertToRadians(angle);
	//float si = sinf(degree);
	//float co = cosf(degree);

	//for (int i = 0; i < 4; i++) {
	//	float pos_x = vertices[i].position.x;
	//	float pos_y = vertices[i].position.y;
	//	vertices[i].position.x = pos_x * co - pos_y * si;
	//	vertices[i].position.y = pos_x * si + pos_y * co;
	//}
	////回転軸の変更(元の位置へ)
	//for (int i = 0; i < 4; i++) {
	//	vertices[i].position.x += texX;
	//	vertices[i].position.y += texY;
	//}

	////頂点バッファの更新
	//D3D11_MAPPED_SUBRESOURCE mapped;
	//D3D11_VIEWPORT viewport;
	//UINT num_viewports = 1;
	//context->RSGetViewports(&num_viewports, &viewport);

	//hr = context->Map(mVSBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
	//_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));


	//Vertex* v = (Vertex*)mapped.pData;

	//for (int i = 0; i < 4; i++) {
	//	v[i].position.x = (vertices[i].position.x / viewport.Width) * 2.0f - 1.0f;
	//	v[i].position.y = -(vertices[i].position.y / viewport.Height) * 2.0f + 1.0f;
	//	v[i].color = vertices[i].color;
	//	v[i].texcoord.x = vertices[i].texcoord.x / texture2dDesc.Width;
	//	v[i].texcoord.y = vertices[i].texcoord.y / texture2dDesc.Height;
	//}

	//context->Unmap(mVSBuffer.Get(), 0);


	//float colors[4] = { 0,0,0,1 };
	//UINT width = static_cast<UINT>(SCREEN_WIDTH);
	//UINT height = static_cast<UINT>(SCREEN_HEIGHT);

	//for (int i = 0;i < 5;i++)
	//{
	//	width /= 4;
	//	height /= 4;

	//	context->ClearRenderTargetView(mRenderTargetView[i].Get(), colors);
	//	context->OMSetRenderTargets(1, mRenderTargetView[i].GetAddressOf(), mDepthStencilView[i].Get());

	//	viewport.TopLeftX = 0;
	//	viewport.TopLeftY = 0;
	//	viewport.Width = static_cast<float>(width);
	//	viewport.Height = static_cast<float>(height);
	//	viewport.MinDepth = 0.0f;
	//	viewport.MaxDepth = 1.0f;
	//	context->RSSetViewports(1, &viewport);

	//	context->IASetVertexBuffers(0, 1, mVSBuffer.GetAddressOf(), &pstrides, &poff);
	//	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	//	context->IASetInputLayout(mInput.Get());
	//	context->RSSetState(mRasterizerState.Get());
	//	context->VSSetShader(mVSShader.Get(), NULL, 0);
	//	context->PSSetShader(mPSShader.Get(), NULL, 0);
	//	context->PSSetSamplers(0, 1, mSamplerState.GetAddressOf());
	//	context->PSSetShaderResources(0, 1, mTexview.GetAddressOf());

	//	context->Draw(4, 0);
	//}
	//width = static_cast<UINT>(SCREEN_WIDTH);
	//height = static_cast<UINT>(SCREEN_HEIGHT);
	//ID3D11RenderTargetView* rtv = Framework::Instance().GetRenderTargetView().Get();
	//context->ClearRenderTargetView(rtv, colors);

	//context->OMSetRenderTargets(1, &rtv, nullptr);
	//viewport.TopLeftX = 0;
	//viewport.TopLeftY = 0;
	//viewport.Width = static_cast<float>(width);
	//viewport.Height = static_cast<float>(height);
	//viewport.MinDepth = 0.0f;
	//viewport.MaxDepth = 1.0f;
	//context->RSSetViewports(1, &viewport);

	//for (int i = 0;i < 5;i++)
	//{
	//	//width /= 4;
	//	//height /= 4;
	//	vertices[0] = { VECTOR3F(0,0,0),VECTOR4F(1,1,1,1),VECTOR2F(0,0) };
	//	vertices[1] = { VECTOR3F(static_cast<UINT>(SCREEN_WIDTH),0,0),VECTOR4F(1,1,1,1),VECTOR2F(width,0) };
	//	vertices[2] = { VECTOR3F(0,static_cast<UINT>(SCREEN_HEIGHT),0),VECTOR4F(1,1,1,1),VECTOR2F(0,height) };
	//	vertices[3] = { VECTOR3F(static_cast<UINT>(SCREEN_WIDTH),static_cast<UINT>(SCREEN_HEIGHT),0),VECTOR4F(1,1,1,1),VECTOR2F(width,height) };
	//	hr = context->Map(mVSBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
	//	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

	//	Vertex* v = (Vertex*)mapped.pData;

	//	for (int i = 0; i < 4; i++) {
	//		v[i].position.x = (vertices[i].position.x / viewport.Width) * 2.0f - 1.0f;
	//		v[i].position.y = -(vertices[i].position.y / viewport.Height) * 2.0f + 1.0f;
	//		v[i].color = vertices[i].color;
	//		v[i].texcoord.x = vertices[i].texcoord.x / width;
	//		v[i].texcoord.y = vertices[i].texcoord.y / height;
	//	}

	//	context->Unmap(mVSBuffer.Get(), 0);
	//	context->IASetVertexBuffers(0, 1, mVSBuffer.GetAddressOf(), &pstrides, &poff);
	//	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	//	context->IASetInputLayout(mInput.Get());
	//	context->RSSetState(mRasterizerState.Get());
	//	context->VSSetShader(mVSShader.Get(), NULL, 0);
	//	context->PSSetShader(mPSShader.Get(), NULL, 0);
	//	context->PSSetSamplers(0, 1, mSamplerState.GetAddressOf());
	//	context->PSSetShaderResources(0, 1, mTexview.GetAddressOf());

	//	context->PSSetShaderResources(0, 1, mShaderResourceView[i].GetAddressOf());
	//	context->Draw(4, 0);
	//}
#if 0
	UINT pstrides = sizeof(Vertex);
	UINT poff = 0;
	CalculateVertex(context, position, size, texPosition, texSize, angle, color);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	context->IASetInputLayout(mInput.Get());
	context->RSSetState(mRasterizerState.Get());
	context->VSSetShader(mVSShader.Get(), NULL, 0);
	context->PSSetShader(mPSShader.Get(), NULL, 0);
	float colors[4] = { 0,0,0,1 };
	float width = static_cast<float>(SCREEN_WIDTH);
	float height = static_cast<float>(SCREEN_HEIGHT);
	auto pDSV = Framework::Instance().GetDepthStencilView().Get();
	auto pRTV = Framework::Instance().GetRenderTargetView().Get();
	for (int i = 0;i < 5;i++)
	{
		pRTV = mRenderTargetView[i].Get();
		//width /= 4;
		//height /= 4;
		context->OMSetRenderTargets(1, &pRTV, nullptr);

		context->ClearRenderTargetView(pRTV, colors);
		//context->ClearDepthStencilView(pDSV, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
		D3D11_VIEWPORT viewport;
		viewport.TopLeftX = 0;
		viewport.TopLeftY = 0;
		viewport.Width = width;
		viewport.Height = height;
		viewport.MinDepth = 0.0f;
		viewport.MaxDepth = 1.0f;
		context->RSSetViewports(1, &viewport);
		context->IASetVertexBuffers(0, 1, mVSBuffer.GetAddressOf(), &pstrides, &poff);
		context->PSSetSamplers(0, 1, mSamplerState.GetAddressOf());
		context->PSSetShaderResources(0, 1, mTexview.GetAddressOf());
		context->OMSetDepthStencilState(mDepthStencilState.Get(), 0);

		context->Draw(4, 0);

	}
	width = static_cast<float>(SCREEN_WIDTH);
	height = static_cast<float>(SCREEN_HEIGHT);
	pRTV = Framework::Instance().GetRenderTargetView().Get();
	context->OMSetRenderTargets(1, &pRTV, nullptr);

	context->ClearRenderTargetView(pRTV, colors);
	//context->ClearDepthStencilView(pDSV, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	D3D11_VIEWPORT viewport;
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = width;
	viewport.Height = height;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	context->RSSetViewports(1, &viewport);
	VECTOR2F p, s;
	p = VECTOR2F(0, 0);
	s = VECTOR2F(static_cast<UINT>(SCREEN_WIDTH), static_cast<UINT>(SCREEN_HEIGHT));
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	context->IASetInputLayout(mInput.Get());
	context->RSSetState(mRasterizerState.Get());
	context->VSSetShader(mVSShader.Get(), NULL, 0);
	context->PSSetShader(mPSShader.Get(), NULL, 0);

	for (int i = 0;i < 1;i++)
	{
		//width /= 4;
		//height /= 4;
		VECTOR2F  tp, ts;
		tp = VECTOR2F(0, 0);
		ts = VECTOR2F(width, height);
		CalculateVertex(context, p, s, tp, ts, 0, color);
		context->IASetVertexBuffers(0, 1, mVSBuffer.GetAddressOf(), &pstrides, &poff);
		context->PSSetSamplers(0, 1, mSamplerState.GetAddressOf());
		context->PSSetShaderResources(0, 1, mShaderResourceView[i].GetAddressOf());
		context->OMSetDepthStencilState(mDepthStencilState.Get(), 0);

		context->Draw(4, 0);

	}
#endif
}

void BlommTest01::CalculateVertex(ID3D11DeviceContext* context, const VECTOR2F& position, const VECTOR2F& size,
	const VECTOR2F& texPosition, const VECTOR2F& texSize, float angle, const VECTOR4F& color)
{
	HRESULT hr;
	Vertex vertices[] =
	{
		{VECTOR3F(position.x,position.y,0),color,VECTOR2F(texPosition.x,texPosition.y)},
		{VECTOR3F(position.x + size.x,position.y,0),color,VECTOR2F(texPosition.x + texSize.x,texPosition.y)},
		{VECTOR3F(position.x,position.y + size.y,0),color,VECTOR2F(texPosition.x,texPosition.y + texSize.y)},
		{VECTOR3F(position.x + size.x,position.y + size.y,0),color,VECTOR2F(texPosition.x + texSize.x,texPosition.y + texSize.y)},
	};

	float texX = position.x + size.x / 2.0f;
	float texY = position.y + size.y / 2.0f;
	//	回転軸の変更(短形中央へ)
	for (int i = 0; i < 4; i++) {
		vertices[i].position.x -= texX;
		vertices[i].position.y -= texY;
	}
	//    回転
	float degree = DirectX::XMConvertToRadians(angle);
	float si = sinf(degree);
	float co = cosf(degree);

	for (int i = 0; i < 4; i++) {
		float pos_x = vertices[i].position.x;
		float pos_y = vertices[i].position.y;
		vertices[i].position.x = pos_x * co - pos_y * si;
		vertices[i].position.y = pos_x * si + pos_y * co;
	}
	//回転軸の変更(元の位置へ)
	for (int i = 0; i < 4; i++) {
		vertices[i].position.x += texX;
		vertices[i].position.y += texY;
	}

	//頂点バッファの更新
	D3D11_MAPPED_SUBRESOURCE mapped;
	D3D11_VIEWPORT viewport;
	UINT num_viewports = 1;
	context->RSGetViewports(&num_viewports, &viewport);

	hr = context->Map(mVSBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));


	Vertex* v = (Vertex*)mapped.pData;

	for (int i = 0; i < 4; i++) {
		v[i].position.x = (vertices[i].position.x / viewport.Width) * 2.0f - 1.0f;
		v[i].position.y = -(vertices[i].position.y / viewport.Height) * 2.0f + 1.0f;
		v[i].color = vertices[i].color;
		v[i].texcoord.x = vertices[i].texcoord.x / texture2dDesc.Width;
		v[i].texcoord.y = vertices[i].texcoord.y / texture2dDesc.Height;
	}

	context->Unmap(mVSBuffer.Get(), 0);
}
