#pragma once
#include <memory>
#include <d3d11.h>
#include "model.h"
#include"drow_shader.h"
#include<vector>
#include"constant_buffer.h"

class ModelRenderer
{
public:
	ModelRenderer(ID3D11Device* device);
	~ModelRenderer()
	{
	}

	void Begin(ID3D11DeviceContext* context, const FLOAT4X4& view_projection);
	void Draw(ID3D11DeviceContext* context, Model& model, const VECTOR4F& color = VECTOR4F(1, 1, 1, 1));
	void Draw(ID3D11DeviceContext* context, DrowShader* shader, Model& model, const VECTOR4F& color = VECTOR4F(1, 1, 1, 1));
	void End(ID3D11DeviceContext* context);

	void ShadowBegin(ID3D11DeviceContext* context, const FLOAT4X4& view_projection);
	void ShadowDraw(ID3D11DeviceContext* context, Model& model, const VECTOR4F& color = VECTOR4F(1, 1, 1, 1));
	void ShadowEnd(ID3D11DeviceContext* context);

	void VelocityBegin(ID3D11DeviceContext* context, const FLOAT4X4& viewProjection);
	void VelocityDraw(ID3D11DeviceContext* context,  Model& model);
	void VelocityEnd(ID3D11DeviceContext* context);

private:
	static const int MaxBones = 128;

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

	std::vector<std::unique_ptr<DrowShader>>mShader;
	std::unique_ptr<DrowShader>mShadowShader;

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>	mDummySRV;
};
