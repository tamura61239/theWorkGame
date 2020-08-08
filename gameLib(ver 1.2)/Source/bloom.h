#pragma once
#include"framebuffer.h"
#include<vector>
#include"vector.h"

class BloomRender
{
public:
	BloomRender(ID3D11Device* device, float screenWidth, float screenHight);
	void ImGuiUpdate();
	void Render(ID3D11DeviceContext* context, ID3D11ShaderResourceView* colorSrv, bool render);
private:
	void Load();
	void Save();
	std::vector<std::unique_ptr<FrameBuffer>>mFrameBuffer;
	Microsoft::WRL::ComPtr<ID3D11PixelShader>mPSShader[3];
	Microsoft::WRL::ComPtr<ID3D11VertexShader>mVSShader;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState>mDepthStencilState;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState>mRasterizeState;
	Microsoft::WRL::ComPtr<ID3D11SamplerState>mSamplerState;
	Microsoft::WRL::ComPtr<ID3D11Buffer>mCBbuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer>mCbBluerbuffer;
	struct CbBloom
	{
		float threshold;
		float widthBlur;
		float hightBlur;
		float blurCount;
	};
	struct CbBluer
	{
		VECTOR4F mOffset[8];
	};
	float GaussianDistribution(const VECTOR2F& position, const float rho);
	void CalucurateBluer(const float width, const float hight, const VECTOR2F& dir, const float deviation, const float multiply);
	CbBloom mCbuffer;
	CbBluer mCbBluer;
	int count;
};