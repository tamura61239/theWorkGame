#pragma once
#include <memory>
#include <d3d11.h>
#include "model.h"
#include"drow_shader.h"
#include<vector>
#include"constant_buffer.h"
//アニメーションできるモデルを描画するクラス
class ModelRenderer
{
public:
	//コンストラクタ
	ModelRenderer(ID3D11Device* device);
	//デストラクタ
	~ModelRenderer()
	{
	}
	//描画
	void Begin(ID3D11DeviceContext* context, const FLOAT4X4& view_projection);
	void Draw(ID3D11DeviceContext* context, Model& model, const VECTOR4F& color = VECTOR4F(1, 1, 1, 1));
	void Draw(ID3D11DeviceContext* context, DrowShader* shader, Model& model, const VECTOR4F& color = VECTOR4F(1, 1, 1, 1));
	void End(ID3D11DeviceContext* context);
	//影の描画
	void ShadowBegin(ID3D11DeviceContext* context);
	void ShadowDraw(ID3D11DeviceContext* context, Model& model, const VECTOR4F& color = VECTOR4F(1, 1, 1, 1));
	void ShadowEnd(ID3D11DeviceContext* context);
	//速度マップの描画
	void VelocityBegin(ID3D11DeviceContext* context);
	void VelocityDraw(ID3D11DeviceContext* context,  Model& model);
	void VelocityEnd(ID3D11DeviceContext* context);

private:
	void SetRenderBoneTransform(const ModelResource::Mesh& mesh, const std::vector<Model::Node>& nodes);
	//ボーンの最大数
	static const int MaxBones = 128;
	//定数バッファ
	struct CbScene
	{
		FLOAT4X4	viewProjection;
	};
	struct CbMesh
	{
		FLOAT4X4	boneTransforms[MaxBones];
		FLOAT4X4	beforeBoneTransforms[MaxBones];
	};
	struct CbSubset
	{
		VECTOR4F	materialColor;
	};
	std::unique_ptr<ConstantBuffer<CbScene>>			mCbScene;
	std::unique_ptr<ConstantBuffer<CbMesh>>			mCbMesh;
	std::unique_ptr<ConstantBuffer<CbSubset>>			mCbSubset;
	//シェーダー
	std::vector<std::unique_ptr<DrowShader>>mShader;
	std::unique_ptr<DrowShader>mShadowShader;
	//ダミーテクスチャ
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>	mDummySRV;
};
