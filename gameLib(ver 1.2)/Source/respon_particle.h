#pragma once
#include"vector.h"
#include<d3d11.h>
#include<wrl.h>
#include<vector>

class ResponParticle
{
public:
	ResponParticle(ID3D11Device* device);
	void Update(ID3D11DeviceContext* context,const VECTOR3F&position);
	void Render(ID3D11DeviceContext* context);
	struct Particle
	{

	};
private:
	//バッファ
	Microsoft::WRL::ComPtr<ID3D11Buffer>mParticleBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer>mCbBuffer;
	//UAV
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView>mParticleUAV;
	//シェーダー
	Microsoft::WRL::ComPtr<ID3D11ComputeShader>mCSShader;
	Microsoft::WRL::ComPtr<ID3D11GeometryShader>mGSShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader>mPSShader;
	Microsoft::WRL::ComPtr<ID3D11VertexShader>mVSShader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout>mInput;
	void Load();
	void Save();

};