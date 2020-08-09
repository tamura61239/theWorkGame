#pragma once
#include <memory>
#include <d3d11.h>
#include "model.h"

class ModelRenderer
{
public:
	ModelRenderer(ID3D11Device* device);
	~ModelRenderer() 
	{
	}

	void Begin(ID3D11DeviceContext* context, const FLOAT4X4& view_projection);
	void Draw(ID3D11DeviceContext* context, Model& model,const VECTOR4F&color= VECTOR4F(1,1,1,1));
	void End(ID3D11DeviceContext* context);

	void ShadowBegin(ID3D11DeviceContext* context, const FLOAT4X4& view_projection);
	void ShadowDraw(ID3D11DeviceContext* context, Model& model, const VECTOR4F& color = VECTOR4F(1, 1, 1, 1));
	void ShadowEnd(ID3D11DeviceContext* context);

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

	Microsoft::WRL::ComPtr<ID3D11VertexShader>		m_vertex_shader[static_cast<int>(SHADER_TYPE::MAX)];
	Microsoft::WRL::ComPtr<ID3D11VertexShader>		mShadowVSShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader>		m_pixel_shader[2];
	Microsoft::WRL::ComPtr<ID3D11GeometryShader>	d_m_g_shader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout>		m_input_layout;

	Microsoft::WRL::ComPtr<ID3D11BlendState>		m_blend_state;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState>	m_rasterizer_state;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState>	m_depth_stencil_state;

	Microsoft::WRL::ComPtr<ID3D11SamplerState>		m_sampler_state[2];
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>	m_dummy_srv;
};
