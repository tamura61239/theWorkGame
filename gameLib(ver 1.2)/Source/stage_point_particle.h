#pragma once
#include"vector.h"
#include<d3d11.h>
#include<wrl.h>
#include<vector>

class StagePointParticle
{
public:
	StagePointParticle(ID3D11Device* device);
	void Update(ID3D11DeviceContext* context, float elapsdTime, ID3D11UnorderedAccessView* uav);
	void Reder(ID3D11DeviceContext* context);
private:
	Microsoft::WRL::ComPtr<ID3D11Buffer>mParticleBuffer;

	Microsoft::WRL::ComPtr<ID3D11Buffer>mCbStartBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer>mCbBuffer;
	struct CbStart
	{
		float startIndex;
	};
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView>mParticleUAV;

	Microsoft::WRL::ComPtr<ID3D11ComputeShader>mCSShader;
	Microsoft::WRL::ComPtr<ID3D11ComputeShader>mCSCreateShader;
	Microsoft::WRL::ComPtr<ID3D11VertexShader>mVSShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader>mPSShader;
	Microsoft::WRL::ComPtr<ID3D11GeometryShader>mGSShader;
};