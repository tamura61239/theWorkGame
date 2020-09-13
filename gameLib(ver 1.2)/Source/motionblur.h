#pragma once
#include<d3d11.h>
#include<memory>
#include<wrl.h>

class MotionBlur
{
public:
	MotionBlur(ID3D11Device* device);
	void CreateNeighborMaxBuffer(ID3D11DeviceContext* context, ID3D11ShaderResourceView* srv);
	void SetPsTexture(ID3D11DeviceContext* context, const int number);
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>GetSRV() { return mSRV; }
private:
	Microsoft::WRL::ComPtr<ID3D11ComputeShader>mCSShader;
	Microsoft::WRL::ComPtr<ID3D11Texture2D>mTexture2d;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>mSRV;
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView>mUAV;
};