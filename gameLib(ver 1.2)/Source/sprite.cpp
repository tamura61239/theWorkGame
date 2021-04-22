#include "sprite.h"
#include "misc.h"
#include<memory>
#include"shader.h"
#include"texture.h"

/*****************************************************/
//　　　　　　　　　　初期化関数
/*****************************************************/
/*************************コンストラクタ(テクスチャを生成する)********************/

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
	// シェーダーを生成する
	D3D11_INPUT_ELEMENT_DESC input_element_desc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }

	};
	mShader = std::make_unique<DrowShader>(device, "Data/shader/sprite_vs.cso", "", "Data/shader/sprite_ps.cso", input_element_desc, ARRAYSIZE(input_element_desc));
	//テクスチャを生成する
	Load(device, fileName);
}
/*************************コンストラクタ(テクスチャを生成しない)********************/
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
}
/******************************テクスチャを読み込む****************************/
void Sprite::Load(ID3D11Device* device, const wchar_t* fileName)
{
	HRESULT hr;

	hr = LoadTextureFromFile(device, fileName, mTexview.GetAddressOf(), &mTexture2d);
	_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

}
/*****************************************************/
//　　　　　　　　　　描画関数
/*****************************************************/

void Sprite::Render(ID3D11DeviceContext* context, const VECTOR2F& position, const VECTOR2F& size, const VECTOR2F& texPosition, const VECTOR2F& texSize, float angle, const VECTOR4F& color)
{
	UINT pstrides = sizeof(Vertex);
	UINT poff = 0;
	HRESULT hr;
	//バッファのデータを更新
	BufferUpdate(context, position, size, texPosition, texSize, angle, color, mTexture2d);
	//GPU側にデータを送る
	mShader->Activate(context);

	context->IASetVertexBuffers(0, 1, mVSBuffer.GetAddressOf(), &pstrides, &poff);

	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	context->PSSetShaderResources(0, 1, mTexview.GetAddressOf());
	//描画
	context->Draw(4, 0);
	//GPU側に送ったデータを元に戻す
	mShader->Deactivate(context);
	ID3D11ShaderResourceView* defSRV = nullptr;
	context->PSSetShaderResources(0, 1, &defSRV);
	ID3D11Buffer* buffer = nullptr;
	pstrides = 0;
	context->IASetVertexBuffers(0, 1, &buffer, &pstrides, &poff);

}

void Sprite::Render(ID3D11DeviceContext* context, ID3D11ShaderResourceView* srv, const VECTOR2F& position, const VECTOR2F& size, const VECTOR2F& texPosition, const VECTOR2F& texSize, float angle, const VECTOR4F& color)
{
	//texture2dDescを取得
	D3D11_TEXTURE2D_DESC texture2d;
	Microsoft::WRL::ComPtr<ID3D11Texture2D>tex2d;
	Microsoft::WRL::ComPtr<ID3D11Resource>resouce;
	srv->GetResource(resouce.GetAddressOf());
	resouce->QueryInterface<ID3D11Texture2D>(tex2d.GetAddressOf());
	tex2d->GetDesc(&texture2d);
	UINT pstrides = sizeof(Vertex);
	UINT poff = 0;
	//バッファのデータを更新
	BufferUpdate(context, position, size, texPosition, texSize, angle, color, texture2d);
	//GPU側にデータを送る

	mShader->Activate(context);

	context->IASetVertexBuffers(0, 1, mVSBuffer.GetAddressOf(), &pstrides, &poff);

	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	context->PSSetShaderResources(0, 1, &srv);
	//描画
	context->Draw(4, 0);
	//GPU側に送ったデータを元に戻す

	mShader->Deactivate(context);
	ID3D11ShaderResourceView* defSRV = nullptr;
	context->PSSetShaderResources(0, 1, &defSRV);
	ID3D11Buffer* buffer = nullptr;
	pstrides = 0;
	context->IASetVertexBuffers(0, 1, &buffer, &pstrides, &poff);


}

void Sprite::Render(ID3D11DeviceContext* context, DrowShader* shader, const VECTOR2F& position, const VECTOR2F& size, const VECTOR2F& texPosition, const VECTOR2F& texSize, float angle, const VECTOR4F& color)
{
	UINT pstrides = sizeof(Vertex);
	UINT poff = 0;
	//バッファのデータを更新
	BufferUpdate(context, position, size, texPosition, texSize, angle, color, mTexture2d);
	//GPU側にデータを送る

	shader->Activate(context);

	context->IASetVertexBuffers(0, 1, mVSBuffer.GetAddressOf(), &pstrides, &poff);

	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	context->PSSetShaderResources(0, 1, mTexview.GetAddressOf());
	//描画
	context->Draw(4, 0);
	//GPU側に送ったデータを元に戻す

	shader->Deactivate(context);
	ID3D11ShaderResourceView*defSRV = nullptr;
	context->PSSetShaderResources(0, 1, &defSRV);
	ID3D11Buffer* buffer = nullptr;
	pstrides = 0;
	context->IASetVertexBuffers(0, 1, &buffer, &pstrides, &poff);

}

void Sprite::Render(ID3D11DeviceContext* context, DrowShader* shader, ID3D11ShaderResourceView* srv, const VECTOR2F& position, const VECTOR2F& size, const VECTOR2F& texPosition, const VECTOR2F& texSize, float angle, const VECTOR4F& color)
{
	//texture2dDescを取得
	D3D11_TEXTURE2D_DESC texture2d;
	Microsoft::WRL::ComPtr<ID3D11Texture2D>tex2d;
	Microsoft::WRL::ComPtr<ID3D11Resource>resouce;
	srv->GetResource(resouce.GetAddressOf());
	resouce->QueryInterface<ID3D11Texture2D>(tex2d.GetAddressOf());
	tex2d->GetDesc(&texture2d);
	UINT pstrides = sizeof(Vertex);
	UINT poff = 0;
	//バッファのデータを更新
	BufferUpdate(context, position, size, texPosition, texSize, angle, color, texture2d);
	//GPU側にデータを送る

	shader->Activate(context);

	context->IASetVertexBuffers(0, 1, mVSBuffer.GetAddressOf(), &pstrides, &poff);

	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	context->PSSetShaderResources(0, 1, &srv);
	//描画
	context->Draw(4, 0);
	//GPU側に送ったデータを元に戻す

	shader->Deactivate(context);
	ID3D11ShaderResourceView* defSRV = nullptr;
	context->PSSetShaderResources(0, 1, &defSRV);
	ID3D11Buffer* buffer = nullptr;
	pstrides = 0;
	context->IASetVertexBuffers(0, 1, &buffer, &pstrides, &poff);

}
/******************************************バッファのデータを更新**************************************/
void Sprite::BufferUpdate(ID3D11DeviceContext* context, const VECTOR2F& position, const VECTOR2F& size
	, const VECTOR2F& texPosition, const VECTOR2F& texSize, float angle, const VECTOR4F& color, D3D11_TEXTURE2D_DESC texture2dDesc)
{
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
		v[i].texcoord.x = vertices[i].texcoord.x / texture2dDesc.Width;
		v[i].texcoord.y = vertices[i].texcoord.y / texture2dDesc.Height;
	}

	context->Unmap(mVSBuffer.Get(), 0);


}
