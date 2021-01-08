#pragma once
#include <memory>
#include <d3d11.h>
#include "model.h"
#include"drow_shader.h"
#include<vector>

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
	};

	struct CbSubset
	{
		VECTOR4F	materialColor;
	};


	Microsoft::WRL::ComPtr<ID3D11Buffer>			mCbScene;
	Microsoft::WRL::ComPtr<ID3D11Buffer>			mCbMesh;
	Microsoft::WRL::ComPtr<ID3D11Buffer>			mCbSubset;
	Microsoft::WRL::ComPtr<ID3D11Buffer>            mCbBeforeMesh;

	std::vector<std::unique_ptr<DrowShader>>mShader;
	std::unique_ptr<DrowShader>mShadowShader;

	Microsoft::WRL::ComPtr<ID3D11BlendState>		mBlendState;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState>	mRasterizerState;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState>	mDepthStencilState;

	Microsoft::WRL::ComPtr<ID3D11SamplerState>		mSamplerState[2];
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>	mDummySRV;
};
