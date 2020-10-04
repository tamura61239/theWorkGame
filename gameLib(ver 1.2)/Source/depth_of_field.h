#pragma once
#include<d3d11.h>
#include<memory>
#include<wrl.h>
#include"vector.h"

class DepthOfField
{
public:
	DepthOfField(ID3D11Device* device);
	void ImGuiUpdate();
	void ConversionDepthTexture(ID3D11DeviceContext* context, ID3D11ShaderResourceView* srv);
	void SetPsTexture(ID3D11DeviceContext* context, const int number);
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>GetSRV() { return mSRV; }
private:
	void Load();
	void Save();
	Microsoft::WRL::ComPtr<ID3D11ComputeShader>mCSShader;
	Microsoft::WRL::ComPtr<ID3D11Texture2D>mTexture2d;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>mSRV;
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView>mUAV;
	Microsoft::WRL::ComPtr<ID3D11Buffer>mCbData;
	struct Cb
	{
		float inFocusMin;
		float inFocusMax;
		VECTOR2F dummy;
	};
	Cb mCb;
	bool averageFlag;
};