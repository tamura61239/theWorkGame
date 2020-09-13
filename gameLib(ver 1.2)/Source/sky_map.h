#pragma once
#include"geometric_primitive.h"
#include"obj3d.h"
#include<memory>

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
	void Render(ID3D11DeviceContext* context, const FLOAT4X4& view, const FLOAT4X4& projection,const VECTOR4F&color);
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
	Microsoft::WRL::ComPtr<ID3D11VertexShader>mVSShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader>mPSShader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout>mInput;
	Microsoft::WRL::ComPtr<ID3D11Buffer>mCbSceneBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer>mCbObjBuffer;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState>mRasterizerState;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState>mDepthStencilState;
	Microsoft::WRL::ComPtr<ID3D11SamplerState>mSapmleState;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>mSRV;
};