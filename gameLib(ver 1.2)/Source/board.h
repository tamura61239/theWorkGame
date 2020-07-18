#pragma once
#include<d3d11.h>
#include"vector.h"
#include <wrl.h>

class Board
{
public:
	Board(ID3D11Device* device, const wchar_t* fileName);
	void Anim(ID3D11DeviceContext* context, const VECTOR2F&texturePosition, const VECTOR2F&textureSize);
	void Render(ID3D11DeviceContext* context, const VECTOR3F& position, const VECTOR3F& scale, const VECTOR3F& angle, const FLOAT4X4& view, const FLOAT4X4& projection,const VECTOR4F&color=VECTOR4F(1,1,1,1));
private:
	Microsoft::WRL::ComPtr<ID3D11VertexShader>mVSShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader>mPSShader;
	Microsoft::WRL::ComPtr<ID3D11Buffer>mVertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer>mCbBuffer;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>mShaderResourceView;
	Microsoft::WRL::ComPtr<ID3D11InputLayout>mInput;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState>mDepthStencilState;
	Microsoft::WRL::ComPtr<ID3D11SamplerState>mSamplerState;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState>mRasterizerState;
	D3D11_TEXTURE2D_DESC texture2d;
	struct Vertex
	{
		VECTOR3F position;
		VECTOR2F texcoord;
		VECTOR4F color;
	};
	struct CbScene
	{
		FLOAT4X4 worldViewProjection;
		VECTOR4F color;
	};
};