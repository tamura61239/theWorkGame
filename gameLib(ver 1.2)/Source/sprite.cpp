#include "sprite.h"
#include "misc.h"
#include<memory>
#include"shader.h"
#include"texture.h"


Sprite::Sprite(ID3D11Device* device, const wchar_t* fileName)
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
	// GPUに頂点データの内容を教えてあげるための設定
	D3D11_INPUT_ELEMENT_DESC input_element_desc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }

	};
	////頂点シェーダー＆頂点入力レイアウトの生成
	//hr = CreateVSFromCso(device, "Data/shader/sprite_vs.cso", mVSShader.GetAddressOf(), mInput.GetAddressOf(), input_element_desc, ARRAYSIZE(input_element_desc));
	//_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	//// ピクセルシェーダーの生成
	//hr = CreatePSFromCso(device, "Data/shader/sprite_ps.cso", mPSShader.GetAddressOf());
	//_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	mShader = std::make_unique<DrowShader>(device, "Data/shader/sprite_vs.cso", "", "Data/shader/sprite_ps.cso", input_element_desc, ARRAYSIZE(input_element_desc));
	hr = LoadTextureFromFile(device, fileName, mTexview.GetAddressOf(), &mTexture2d);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

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

	//SamplerStateの生成
	{
		D3D11_SAMPLER_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		desc.ComparisonFunc = D3D11_COMPARISON_NEVER;
		desc.MinLOD = -FLT_MAX;
		desc.MaxLOD = FLT_MAX;
		desc.MaxAnisotropy = 16;
		memcpy(desc.BorderColor, &VECTOR4F(.0f, .0f, .0f, .0f), sizeof(VECTOR4F));
		hr = device->CreateSamplerState(&desc, mSamplerState.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}
	//DepthStencilStateの生成
	{
		D3D11_DEPTH_STENCIL_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.DepthEnable = true;
		desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		desc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
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
}

Sprite::Sprite(ID3D11Device* device)
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
	// GPUに頂点データの内容を教えてあげるための設定
	D3D11_INPUT_ELEMENT_DESC input_element_desc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }

	};
	mShader = std::make_unique<DrowShader>(device, "Data/shader/sprite_vs.cso", "", "Data/shader/sprite_ps.cso", input_element_desc, ARRAYSIZE(input_element_desc));

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

	//SamplerStateの生成
	{
		D3D11_SAMPLER_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
		desc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
		desc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
		desc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
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
		desc.DepthEnable = true;
		desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		desc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

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

}

void Sprite::Load(ID3D11Device* device, const wchar_t* fileName)
{
	HRESULT hr;

	hr = LoadTextureFromFile(device, fileName, mTexview.GetAddressOf(), &mTexture2d);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

}

void Sprite::Render(ID3D11DeviceContext* context, const VECTOR2F& position, const VECTOR2F& size, const VECTOR2F& texPosition, const VECTOR2F& texSize, float angle, const VECTOR4F& color)
{
	UINT pstrides = sizeof(Vertex);
	UINT poff = 0;
	HRESULT hr;
	//D3D11_MAP map = D3D11_MAP_WRITE_DISCARD;
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
		v[i].texcoord.x = vertices[i].texcoord.x / mTexture2d.Width;
		v[i].texcoord.y = vertices[i].texcoord.y / mTexture2d.Height;
	}

	context->Unmap(mVSBuffer.Get(), 0);

	mShader->Activate(context);

	context->IASetVertexBuffers(0, 1, mVSBuffer.GetAddressOf(), &pstrides, &poff);

	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	context->RSSetState(mRasterizerState.Get());
	context->PSSetShaderResources(0, 1, mTexview.GetAddressOf());
	context->PSSetSamplers(0, 1, mSamplerState.GetAddressOf());
	context->OMSetDepthStencilState(mDepthStencilState.Get(), 0);

	context->Draw(4, 0);
	mShader->Deactivate(context);
	context->OMSetDepthStencilState(nullptr, 0);
	context->RSSetState(nullptr);
	ID3D11ShaderResourceView* defSRV = nullptr;
	ID3D11SamplerState* defSampler = nullptr;
	context->PSSetShaderResources(0, 1, &defSRV);
	context->PSSetSamplers(0, 1, &defSampler);

}

void Sprite::Render(ID3D11DeviceContext* context, ID3D11ShaderResourceView* srv, const VECTOR2F& position, const VECTOR2F& size, const VECTOR2F& texPosition, const VECTOR2F& texSize, float angle, const VECTOR4F& color)
{
	D3D11_TEXTURE2D_DESC texture2d;

	Microsoft::WRL::ComPtr<ID3D11Texture2D>tex2d;
	Microsoft::WRL::ComPtr<ID3D11Resource>resouce;
	srv->GetResource(resouce.GetAddressOf());
	resouce->QueryInterface<ID3D11Texture2D>(tex2d.GetAddressOf());
	tex2d->GetDesc(&texture2d);
	UINT pstrides = sizeof(Vertex);
	UINT poff = 0;
	HRESULT hr;
	//D3D11_MAP map = D3D11_MAP_WRITE_DISCARD;
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
		v[i].texcoord.x = vertices[i].texcoord.x / texture2d.Width;
		v[i].texcoord.y = vertices[i].texcoord.y / texture2d.Height;
	}

	context->Unmap(mVSBuffer.Get(), 0);

	mShader->Activate(context);

	context->IASetVertexBuffers(0, 1, mVSBuffer.GetAddressOf(), &pstrides, &poff);

	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	context->RSSetState(mRasterizerState.Get());
	context->PSSetShaderResources(0, 1, &srv);
	context->PSSetSamplers(0, 1, mSamplerState.GetAddressOf());
	context->OMSetDepthStencilState(mDepthStencilState.Get(), 0);

	context->Draw(4, 0);
	mShader->Deactivate(context);
	context->OMSetDepthStencilState(nullptr, 0);
	context->RSSetState(nullptr);
	ID3D11ShaderResourceView* defSRV = nullptr;
	ID3D11SamplerState* defSampler = nullptr;
	context->PSSetShaderResources(0, 1, &defSRV);
	context->PSSetSamplers(0, 1, &defSampler);

}

void Sprite::Render(ID3D11DeviceContext* context, DrowShader* shader, const VECTOR2F& position, const VECTOR2F& size, const VECTOR2F& texPosition, const VECTOR2F& texSize, float angle, const VECTOR4F& color)
{
	UINT pstrides = sizeof(Vertex);
	UINT poff = 0;
	HRESULT hr;
	//D3D11_MAP map = D3D11_MAP_WRITE_DISCARD;
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
		v[i].texcoord.x = vertices[i].texcoord.x / mTexture2d.Width;
		v[i].texcoord.y = vertices[i].texcoord.y / mTexture2d.Height;
	}

	context->Unmap(mVSBuffer.Get(), 0);

	shader->Activate(context);

	context->IASetVertexBuffers(0, 1, mVSBuffer.GetAddressOf(), &pstrides, &poff);

	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	context->RSSetState(mRasterizerState.Get());
	context->PSSetShaderResources(0, 1, mTexview.GetAddressOf());
	context->PSSetSamplers(0, 1, mSamplerState.GetAddressOf());
	context->OMSetDepthStencilState(mDepthStencilState.Get(), 0);

	context->Draw(4, 0);
	shader->Deactivate(context);
	context->OMSetDepthStencilState(nullptr, 0);
	context->RSSetState(nullptr);
	ID3D11ShaderResourceView*defSRV = nullptr;
	ID3D11SamplerState* defSampler = nullptr;
	context->PSSetShaderResources(0, 1, &defSRV);
	context->PSSetSamplers(0, 1, &defSampler);
}

void Sprite::Render(ID3D11DeviceContext* context, DrowShader* shader, ID3D11ShaderResourceView* srv, const VECTOR2F& position, const VECTOR2F& size, const VECTOR2F& texPosition, const VECTOR2F& texSize, float angle, const VECTOR4F& color)
{
	D3D11_TEXTURE2D_DESC texture2d;

	Microsoft::WRL::ComPtr<ID3D11Texture2D>tex2d;
	Microsoft::WRL::ComPtr<ID3D11Resource>resouce;
	srv->GetResource(resouce.GetAddressOf());
	resouce->QueryInterface<ID3D11Texture2D>(tex2d.GetAddressOf());
	tex2d->GetDesc(&texture2d);
	UINT pstrides = sizeof(Vertex);
	UINT poff = 0;
	HRESULT hr;
	//D3D11_MAP map = D3D11_MAP_WRITE_DISCARD;
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
		v[i].texcoord.x = vertices[i].texcoord.x / texture2d.Width;
		v[i].texcoord.y = vertices[i].texcoord.y / texture2d.Height;
	}

	context->Unmap(mVSBuffer.Get(), 0);

	shader->Activate(context);

	context->IASetVertexBuffers(0, 1, mVSBuffer.GetAddressOf(), &pstrides, &poff);

	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	context->RSSetState(mRasterizerState.Get());
	context->PSSetShaderResources(0, 1, &srv);
	context->PSSetSamplers(0, 1, mSamplerState.GetAddressOf());
	context->OMSetDepthStencilState(mDepthStencilState.Get(), 0);

	context->Draw(4, 0);
	shader->Deactivate(context);
	context->OMSetDepthStencilState(nullptr, 0);
	context->RSSetState(nullptr);
	ID3D11ShaderResourceView* defSRV = nullptr;
	ID3D11SamplerState* defSampler = nullptr;
	context->PSSetShaderResources(0, 1, &defSRV);
	context->PSSetSamplers(0, 1, &defSampler);

}
