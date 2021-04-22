#pragma once
#include<d3d11.h>
#include"vector.h"
#include <wrl.h>
#include"drow_shader.h"
#include"constant_buffer.h"
#include<memory>
//テクスチャを張れる板ポリクラス
class Board
{
public:
	//コンストラクタ
	Board(ID3D11Device* device, const wchar_t* fileName);
	//UVアニメーション
	void Anim(ID3D11DeviceContext* context, const VECTOR2F&texturePosition, const VECTOR2F&textureSize);
	//描画
	void Render(ID3D11DeviceContext* context, const VECTOR3F& position, const VECTOR3F& scale, const VECTOR3F& angle, const FLOAT4X4& view, const FLOAT4X4& projection,const VECTOR4F&color=VECTOR4F(1,1,1,1));
private:
	//頂点バッファ
	struct Vertex
	{
		VECTOR3F position;
		VECTOR2F texcoord;
		VECTOR4F color;
	};
	Microsoft::WRL::ComPtr<ID3D11Buffer>mVertexBuffer;
	//定数バッファ
	struct CbScene
	{
		FLOAT4X4 worldViewProjection;
		VECTOR4F color;
	};
	std::unique_ptr<ConstantBuffer<CbScene>>mCbBuffer;
	//テクスチャデータ
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>mShaderResourceView;
	D3D11_TEXTURE2D_DESC texture2d;
	//シェーダー
	std::unique_ptr<DrowShader>mShader;

};