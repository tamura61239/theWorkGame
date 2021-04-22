#include "board.h"
#include"misc.h"
#include"texture.h"
#include"shader.h"

/*****************************************************/
//　　　　　　　　　　初期化関数(コンストラクタ)
/*****************************************************/

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
		mCbBuffer = std::make_unique<ConstantBuffer<CbScene>>(device);
	}
	//シェーダーの生成
	{
		D3D11_INPUT_ELEMENT_DESC inputElementDesc[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
		};
		mShader = std::make_unique<DrowShader>(device, "Data/shader/board_vs.cso", "", "Data/shader/board_ps.cso", inputElementDesc, ARRAYSIZE(inputElementDesc));
	}
	//描画に使うテクスチャの生成
	{
		LoadTextureFromFile(device, fileName, mShaderResourceView.GetAddressOf(), &texture2d);
	}
}
/*****************************************************/
//　　　　　　　　　　UVアニメーション関数
/*****************************************************/

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
/*****************************************************/
//　　　　　　　　　　描画関数
/*****************************************************/

void Board::Render(ID3D11DeviceContext* context, const VECTOR3F& position, const VECTOR3F& scale, const VECTOR3F& angle, const FLOAT4X4& view, const FLOAT4X4& projection, const VECTOR4F& color)
{
	//GPU側にバッファを送る
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	context->IASetVertexBuffers(0, 1, mVertexBuffer.GetAddressOf(), &stride, &offset);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	//シェーダーを設定する
	mShader->Activate(context);
	//GPU側にテクスチャを送る
	context->PSSetShaderResources(0, 1, mShaderResourceView.GetAddressOf());

	{
		//ワールド空間行列を計算する
		DirectX::XMMATRIX W;
		{
			DirectX::XMMATRIX S, R, T;
			S = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);
			R = DirectX::XMMatrixRotationRollPitchYaw(angle.x, angle.y, angle.z);
			T = DirectX::XMMatrixTranslation(position.x, position.y, position.z);
			W = S * R * T;
		}
		//view projection行列
		DirectX::XMMATRIX V, P;
		V = DirectX::XMLoadFloat4x4(&view);
		P = DirectX::XMLoadFloat4x4(&projection);
		//view行列の逆行列を生成する
		FLOAT4X4 inverseView = view;
		inverseView._41 = inverseView._42 = inverseView._43 = 0.0f;
		inverseView._44 = 1.0f;
		DirectX::XMMATRIX inverseMatrixView;
		inverseMatrixView = DirectX::XMLoadFloat4x4(&inverseView);
		//ビルボード用の行列作成
		DirectX::XMStoreFloat4x4(&mCbBuffer->data.worldViewProjection, inverseMatrixView * W * V * P);
		mCbBuffer->data.color = color;
	}
	mCbBuffer->Activate(context, 0, true, true);

	context->Draw(4, 0);
	mShader->Deactivate(context);
	mCbBuffer->DeActivate(context);

}
