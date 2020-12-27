#pragma once
#include<d3d11.h>
#include<wrl.h>
#include"vector.h"
#include"drow_shader.h"
#include<memory>
#include<string>

class RenderEffects
{
public:
	RenderEffects(ID3D11Device* device,std::string fileName="");
	void ImGuiUpdate();
	void ShadowRender(ID3D11DeviceContext* context, ID3D11ShaderResourceView* colorMapSRV, ID3D11ShaderResourceView* depthMapSRV, ID3D11ShaderResourceView* shadowMapSRV
		,const FLOAT4X4&view,const FLOAT4X4&projection,const FLOAT4X4&lightView,const FLOAT4X4&lightProjection);
	void DeferrdShadowRender(ID3D11DeviceContext* context, DrowShader* shader, const FLOAT4X4& view, const FLOAT4X4& projection, const FLOAT4X4& lightView, const FLOAT4X4& lightProjection);

private:
	void Load(std::string fileName);
	void Save(std::string fileName);
	std::unique_ptr<DrowShader>mShader;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState>mRasterizerState;
	Microsoft::WRL::ComPtr<ID3D11SamplerState>mSamplerState[2];
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState>mDepthStencilState;
	Microsoft::WRL::ComPtr<ID3D11Buffer>mCbBuffer;
	struct SaveData
	{
		VECTOR3F shadowColor = VECTOR3F(0.65f, 0.65f, 0.65f);
		float shadowbisa = 0.0008f;
		float slopeScaledBias = 0;
		float depthBiasClamp = 0;
		VECTOR2F dummy;
	};
	struct CbScene
	{
		FLOAT4X4 inverseViewProjection;
		FLOAT4X4 lightViewProjection;
		SaveData data;
	};
	std::string mFileName = "";
	CbScene mCbScene;

};