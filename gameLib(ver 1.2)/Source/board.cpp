#include "board.h"
#include"misc.h"
#include"texture.h"
#include"shader.h"

Board::Board(ID3D11Device* device, const wchar_t* fileName)
{
	HRESULT hr;
	Vertex vertices[] =
	{
		{VECTOR3F(0.5f,0.5f,0.0f),VECTOR2F(1,0),VECTOR4F(1,1,1,1)},
		{VECTOR3F(-0.5f,0.5f,0.0f),VECTOR2F(0,0),VECTOR4F(1,1,1,1)},
		{VECTOR3F(0.5f,-0.5f,0.0f),VECTOR2F(1,1),VECTOR4F(1,1,1,1)},
		{VECTOR3F(-0.5f,-0.5f,0.0f),VECTOR2F(0,1),VECTOR4F(1,1,1,1)},
	};
	//頂点バッファの生成
	{
		CD3D11_BUFFER_DESC desc = {};
		desc.Usage = D3D11_USAGE_DYNAMIC;
		desc.ByteWidth = sizeof(Vertex) * 4;
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		desc.MiscFlags = 0;
		desc.StructureByteStride = 0;
		D3D11_SUBRESOURCE_DATA data = {};
		data.pSysMem = vertices;
		data.SysMemPitch = 0;
		data.SysMemSlicePitch = 0;
		hr = device->CreateBuffer(&desc, &data, mVertexBuffer.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}
	//定数バッファの生成
	{
		D3D11_BUFFER_DESC desc = {};
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.ByteWidth = sizeof(CbScene);
		desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = 0;
		desc.StructureByteStride = 0;
		hr = device->CreateBuffer(&desc, nullptr, mCbBuffer.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}
	//シェーダーの生成
	{
		D3D11_INPUT_ELEMENT_DESC inputElementDesc[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
		};
		hr = create_vs_from_cso(device, "Data/shader/board_vs.cso", mVSShader.GetAddressOf(), mInput.GetAddressOf(), inputElementDesc, ARRAYSIZE(inputElementDesc));
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
		hr = create_ps_from_cso(device, "Data/shader/board_ps.cso", mPSShader.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}
	//ラスタライザーステートの設定
	{
		D3D11_RASTERIZER_DESC rasterizerDesc;
		ZeroMemory(&rasterizerDesc, sizeof(D3D11_RASTERIZER_DESC));
		rasterizerDesc.FillMode = D3D11_FILL_SOLID;
		rasterizerDesc.CullMode = D3D11_CULL_BACK;
		rasterizerDesc.FrontCounterClockwise = true;
		rasterizerDesc.DepthBiasClamp = 0;
		rasterizerDesc.DepthBias = 0;
		rasterizerDesc.SlopeScaledDepthBias = 0.0f;
		rasterizerDesc.DepthClipEnable = true;
		rasterizerDesc.ScissorEnable = false;
		rasterizerDesc.MultisampleEnable = false;
		rasterizerDesc.AntialiasedLineEnable = false;
		hr = device->CreateRasterizerState(&rasterizerDesc, mRasterizerState.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}
	//サンプラーステートの設定
	{
		D3D11_SAMPLER_DESC samplerDesc;
		ZeroMemory(&samplerDesc, sizeof(D3D11_SAMPLER_DESC));
		samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
		samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
		samplerDesc.MinLOD = -FLT_MAX;
		samplerDesc.MaxLOD = FLT_MAX;
		samplerDesc.MaxAnisotropy = 16;
		memcpy(samplerDesc.BorderColor, &VECTOR4F(1.0f, 1.0f, 1.0f, 1.0f), sizeof(VECTOR4F));
		hr = device->CreateSamplerState(&samplerDesc, mSamplerState.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}
	// 深度ステンシルステートの設定
	{
		//D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
		//ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));
		//depthStencilDesc.DepthEnable = TRUE;
		//depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
		//depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
		//depthStencilDesc.StencilEnable = FALSE;
		//depthStencilDesc.StencilReadMask = 0xFF;
		//depthStencilDesc.StencilWriteMask = 0xFF;
		//depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		//depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
		//depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		//depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
		//depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		//depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
		//depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		//depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
		D3D11_DEPTH_STENCIL_DESC desc;
		::memset(&desc, 0, sizeof(desc));
		desc.DepthEnable = true;
		desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		desc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

		hr = device->CreateDepthStencilState(&desc, mDepthStencilState.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}
	//shaderResouceViewの生成
	{
		load_texture_from_file(device, fileName, mShaderResourceView.GetAddressOf(), &texture2d);
	}
}

void Board::Anim(ID3D11DeviceContext* context, const VECTOR2F& texturePosition, const VECTOR2F& textureSize)
{
	//テクスチャ座標の更新
	HRESULT hr;
	D3D11_MAPPED_SUBRESOURCE mapped;
	Vertex vertices[] =
	{
		{VECTOR3F(0.5f,0.5f,0.0f),VECTOR2F(texturePosition.x + textureSize.x,texturePosition.y),VECTOR4F(1,1,1,1)},
		{VECTOR3F(-0.5f,0.5f,0.0f),VECTOR2F(texturePosition.x,texturePosition.y),VECTOR4F(1,1,1,1)},
		{VECTOR3F(0.5f,-0.5f,0.0f),VECTOR2F(texturePosition.x + textureSize.x,texturePosition.y + textureSize.y),VECTOR4F(1,1,1,1)},
		{VECTOR3F(-0.5f,-0.5f,0.0f),VECTOR2F(texturePosition.x,texturePosition.y + textureSize.y),VECTOR4F(1,1,1,1)},
	};
	hr = context->Map(mVertexBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	Vertex* v = (Vertex*)mapped.pData;
	for (int i = 0;i < 4;i++)
	{
		v[i].position = vertices[i].position;
		v[i].color = vertices[i].color;
		v[i].texcoord.x = vertices[i].texcoord.x / texture2d.Width;
		v[i].texcoord.y = vertices[i].texcoord.y / texture2d.Height;
	}
	context->Unmap(mVertexBuffer.Get(), 0);
}

void Board::Render(ID3D11DeviceContext* context, const VECTOR3F& position, const VECTOR3F& scale, const VECTOR3F& angle, const FLOAT4X4& view, const FLOAT4X4& projection, const VECTOR4F& color)
{
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	context->IASetVertexBuffers(0, 1, mVertexBuffer.GetAddressOf(), &stride, &offset);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	context->IASetInputLayout(mInput.Get());

	context->RSSetState(mRasterizerState.Get());

	context->VSSetShader(mVSShader.Get(), nullptr, 0);

	context->PSSetShader(mPSShader.Get(), nullptr, 0);
	context->PSSetSamplers(0, 1, mSamplerState.GetAddressOf());
	context->PSSetShaderResources(0, 1, mShaderResourceView.GetAddressOf());

	context->OMSetDepthStencilState(mDepthStencilState.Get(), 0);
	{
		DirectX::XMMATRIX W;
		{
			DirectX::XMMATRIX S, R, T;
			S = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);
			R = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);
			T = DirectX::XMMatrixTranslation(position.x, position.y, position.z);
			W = S * R * T;
		}
		DirectX::XMMATRIX V, P;
		V = DirectX::XMLoadFloat4x4(&view);
		P = DirectX::XMLoadFloat4x4(&projection);
		FLOAT4X4 inverseView = view;
		inverseView._41 = inverseView._42 = inverseView._43 = 0.0f;
		inverseView._44 = 1.0f;
		DirectX::XMMATRIX inverseMatrixView;
		inverseMatrixView = DirectX::XMLoadFloat4x4(&inverseView);
		CbScene cbScene;
		DirectX::XMStoreFloat4x4(&cbScene.worldViewProjection, inverseMatrixView * W * V * P);
		cbScene.color = color;
		context->UpdateSubresource(mCbBuffer.Get(), 0, nullptr, &cbScene, 0, 0);
		context->VSSetConstantBuffers(0, 1, mCbBuffer.GetAddressOf());
		context->PSSetConstantBuffers(0, 1, mCbBuffer.GetAddressOf());
	}
	context->Draw(4, 0);
}
