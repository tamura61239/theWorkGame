#pragma once
#include"framebuffer.h"
#include<vector>

class BloomRender
{
public:
	BloomRender(ID3D11Device* device, float screenWidth, float screenHight);
	void ImGuiUpdate();
	void Render(ID3D11DeviceContext* context, ID3D11ShaderResourceView* colorSrv, bool render);
private:
	std::vector<std::unique_ptr<FrameBuffer>>mFrameBuffer;
	Microsoft::WRL::ComPtr<ID3D11PixelShader>mPSShader[3];
	Microsoft::WRL::ComPtr<ID3D11VertexShader>mVSShader;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState>mDepthStencilState;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState>mRasterizeState;
	Microsoft::WRL::ComPtr<ID3D11SamplerState>mSamplerState;
	Microsoft::WRL::ComPtr<ID3D11Buffer>mCBbuffer;
	struct CbScene
	{
		float threshold;
		float widthBlur;
		float hightBlur;
		float blurCount;
	};
	CbScene mCbuffer;
	int count;
};