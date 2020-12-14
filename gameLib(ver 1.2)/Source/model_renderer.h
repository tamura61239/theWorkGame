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
		FLOAT4X4	view_projection;
	};
	struct CbMesh
	{
		FLOAT4X4	bone_transforms[MaxBones];
	};

	struct CbSubset
	{
		VECTOR4F	material_color;
	};


	Microsoft::WRL::ComPtr<ID3D11Buffer>			m_cb_scene;
	Microsoft::WRL::ComPtr<ID3D11Buffer>			m_cb_mesh;
	Microsoft::WRL::ComPtr<ID3D11Buffer>			m_cb_subset;
	Microsoft::WRL::ComPtr<ID3D11Buffer>            mCbBeforeMesh;

	std::vector<std::unique_ptr<DrowShader>>mShader;
	std::unique_ptr<DrowShader>mShadowShader;

	Microsoft::WRL::ComPtr<ID3D11BlendState>		m_blend_state;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState>	m_rasterizer_state;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState>	m_depth_stencil_state;

	Microsoft::WRL::ComPtr<ID3D11SamplerState>		m_sampler_state[2];
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>	m_dummy_srv;
};
