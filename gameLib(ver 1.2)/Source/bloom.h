#pragma once
#include"framebuffer.h"
#include<vector>
#include"vector.h"

class BloomRender
{
public:
	BloomRender(ID3D11Device* device, float screenWidth, float screenHight,const int nowScene);
	void ImGuiUpdate();
	void Render(ID3D11DeviceContext* context, ID3D11ShaderResourceView* colorSrv, bool render);
	//setter
	void SetNowScene(const int nowScene) 
	{ 
		mNowScene = nowScene; 
		mNowEditorNo = mNowScene;
	}
private:
	void Load(const int scene);
	void Save(const int scene);
	void Blur01(ID3D11DeviceContext* context);
	void Blur02(ID3D11DeviceContext* context);
	std::vector<std::unique_ptr<FrameBuffer>>mFrameBuffer;
	std::vector<std::unique_ptr<FrameBuffer>>mSidoFrameBuffer;
	Microsoft::WRL::ComPtr<ID3D11PixelShader>mPSShader[2];
	Microsoft::WRL::ComPtr<ID3D11PixelShader>mPSBlurShader[2];
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
		VECTOR4F mOffset[16];
	};
	struct EditorData
	{
		float threshold;
		float widthBlur;
		float hightBlur;
		float blurCount;
		int mBlurType;
		float deviation;
		float multiply;
		int count;
	};
	float GaussianDistribution(const VECTOR2F& position, const float rho);
	void CalucurateBluer(const float width, const float hight, const VECTOR2F& dir, const float deviation, const float multiply);
	CbBluer mCbBluer;
	EditorData mEditorData[4];
	int mNowEditorNo;
	int mNowScene;
};