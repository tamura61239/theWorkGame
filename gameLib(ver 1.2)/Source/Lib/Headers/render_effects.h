#pragma once
#include<d3d11.h>
#include<wrl.h>
#include"vector.h"
#include"drow_shader.h"
#include<memory>
#include<string>
#include"constant_buffer.h"
//シャドウマップクラス
class RenderEffects
{
public:
	//コンストラクタ
	RenderEffects(ID3D11Device* device,std::string fileName="");
	//エディタ
	void Editor();
	//シーンに影を付ける
	void ShadowRender(ID3D11DeviceContext* context, ID3D11ShaderResourceView* colorMapSRV, ID3D11ShaderResourceView* depthMapSRV, ID3D11ShaderResourceView* shadowMapSRV
		,const FLOAT4X4&view,const FLOAT4X4&projection,const FLOAT4X4&lightView,const FLOAT4X4&lightProjection);
	void ShadowRender(ID3D11DeviceContext* context, DrowShader*shader,ID3D11ShaderResourceView* colorMapSRV, ID3D11ShaderResourceView* depthMapSRV, ID3D11ShaderResourceView* shadowMapSRV
		, const FLOAT4X4& view, const FLOAT4X4& projection, const FLOAT4X4& lightView, const FLOAT4X4& lightProjection);

private:
	//エディタで操作してセーブするデータ
	struct SaveData
	{
		VECTOR3F shadowColor = VECTOR3F(0.65f, 0.65f, 0.65f);
		float shadowbisa = 0.0008f;
		float slopeScaledBias = 0;
		float depthBiasClamp = 0;
		VECTOR2F dummy;
	};
	//定数バッファ
	struct CbScene
	{
		FLOAT4X4 inverseViewProjection;
		FLOAT4X4 lightViewProjection;
		SaveData saveData;
	};
	std::unique_ptr<ConstantBuffer<CbScene>>mCbBuffer;
	//シェーダー
	std::unique_ptr<DrowShader>mShader;
	//ファイルの名前
	std::string mFileName = "";

};