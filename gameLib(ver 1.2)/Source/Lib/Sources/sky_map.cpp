#include "sky_map.h"
#include"misc.h"
#include"shader.h"
#include"texture.h"
/*****************************************************/
//　　　　　　　　　　初期化関数(コンストラクタ)
/*****************************************************/

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
	//シェーダーを生成する
	D3D11_INPUT_ELEMENT_DESC input_element_desc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	mShader = std::make_unique<DrowShader>(device, "Data/shader/sky_map_vs.cso", "", "Data/shader/sky_map_ps.cso", input_element_desc, ARRAYSIZE(input_element_desc));
	//テクスチャを生成する
	LoadTextureFromFile(device, textureName, mSRV.GetAddressOf());
}
/*****************************************************/
//　　　　　　　　　　描画関数
/*****************************************************/
/************************描画する*****************************/

void SkyMap::Render(ID3D11DeviceContext* context, const VECTOR4F& color)
{
	//定数バッファのデータを更新

	mCbObjBuffer->data.color = color * mPosData->GetColor();
	mCbObjBuffer->data.world = mPosData->GetWorld();
	//GPU側にデータを送る

	mCbObjBuffer->Activate(context, 1, true, true);
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
	//描画

	context->DrawIndexed(mObjData->GetIndexNum(), 0, 0);
	//GPU側に送ったデータを元に戻す

	ID3D11ShaderResourceView* srv = nullptr;
	context->PSSetShaderResources(0, 1, &srv);
	mShader->Deactivate(context);
	mCbObjBuffer->DeActivate(context);
	stride = 0;
	ID3D11Buffer* vertexBuffer = nullptr;
	context->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	context->IASetIndexBuffer(nullptr, DXGI_FORMAT_R32_UINT, 0);
	mRasterizer->DeActivate(context);
	mSampler->DeActivate(context);
}
/*************************描画する(シェーダーを取得)****************************/

void SkyMap::Render(ID3D11DeviceContext* context, DrowShader* shader, const VECTOR4F& color)
{
	//定数バッファのデータを更新
	mCbObjBuffer->data.color = color * mPosData->GetColor();
	mCbObjBuffer->data.world = mPosData->GetWorld();
	//GPU側にデータを送る
	mCbObjBuffer->Activate(context, 1, true, true);
	mCbBeforeObjBuffer->Activate(context, 2, true, true);
	context->PSSetShaderResources(0, 1, mSRV.GetAddressOf());
	shader->Activate(context);
	mRasterizer->Activate(context);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	UINT stride = sizeof(GeometricPrimitive::Vertex);
	UINT offset = 0;
	context->IASetVertexBuffers(0, 1, mObjData->GetVertexBuffer().GetAddressOf(), &stride, &offset);
	context->IASetIndexBuffer(mObjData->GetIndexBuffer().Get(), DXGI_FORMAT_R32_UINT, 0);
	//描画
	context->DrawIndexed(mObjData->GetIndexNum(), 0, 0);
	//GPU側に送ったデータを元に戻す
	shader->Deactivate(context);
	mCbObjBuffer->DeActivate(context);
	mCbBeforeObjBuffer->DeActivate(context);

	stride = 0;
	context->IASetVertexBuffers(0, 1, nullptr, &stride, &offset);
	context->IASetIndexBuffer(nullptr, DXGI_FORMAT_R32_UINT, 0);
	mRasterizer->DeActivate(context);
}
