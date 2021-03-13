#pragma once
#include"geometric_primitive.h"
#include"obj3d.h"
#include<memory>
#include"drow_shader.h"
#include"constant_buffer.h"

enum class MAPTYPE
{
	BOX,
	SPHERE
};

class SkyMap
{
public:
	SkyMap(ID3D11Device* device, const wchar_t* textureName, MAPTYPE mapType);
	Obj3D* GetPosData() { return mPosData.get(); }
	void Render(ID3D11DeviceContext* context, const FLOAT4X4& view, const FLOAT4X4& projection, const VECTOR4F& color = VECTOR4F(1, 1, 1, 1));
	void Render(ID3D11DeviceContext* context, DrowShader* shader, const FLOAT4X4& view, const FLOAT4X4& projection, const VECTOR4F& color = VECTOR4F(1, 1, 1, 1));
	void SaveBeforeWorld()
	{
		mCbBeforeObjBuffer->data = mPosData->GetWorld();
	}
private:
	struct Cb
	{
		VECTOR4F color;
		FLOAT4X4 world;

	};
	struct CbScene
	{
		FLOAT4X4 view;
		FLOAT4X4 projection;
	};
	std::unique_ptr<Obj3D>mPosData;
	std::unique_ptr<GeometricPrimitive>mObjData;
	std::unique_ptr<DrowShader>mShader;
	std::unique_ptr<ConstantBuffer<CbScene>>mCbSceneBuffer;
	std::unique_ptr<ConstantBuffer<Cb>>mCbObjBuffer;
	std::unique_ptr<ConstantBuffer<FLOAT4X4>>mCbBeforeObjBuffer;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState>mRasterizerState;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState>mDepthStencilState;
	Microsoft::WRL::ComPtr<ID3D11SamplerState>mSapmleState;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>mSRV;
};