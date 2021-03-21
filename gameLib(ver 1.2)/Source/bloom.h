#pragma once
#include"framebuffer.h"
#include<vector>
#include"vector.h"
#include"constant_buffer.h"
#include"drow_shader.h"
#include"render_state.h"
#include"sampler_state.h"

class BloomRender
{
public:
	BloomRender(ID3D11Device* device, float screenWidth, float screenHight,const int nowScene);
	void ImGuiUpdate();
	void BlurTexture(ID3D11DeviceContext* context, ID3D11ShaderResourceView* colorSrv);
	void Render(ID3D11DeviceContext* context, ID3D11ShaderResourceView* colorSrv);
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

	std::vector<std::unique_ptr<FrameBuffer>>mFrameBuffer;
	std::vector<std::unique_ptr<FrameBuffer>>mSidoFrameBuffer;
	std::vector<std::unique_ptr<DrowShader>>mShader;
	std::unique_ptr<ConstantBuffer<CbBloom>>mCBbuffer;
	std::unique_ptr<ConstantBuffer<CbBluer>>mCbBluerbuffer;
	std::unique_ptr<DepthStencilState>mDepth;
	std::unique_ptr<RasterizerState>mRasterizer;
	enum SamplerType
	{
		warp, anisotropic,max
	};
	std::unique_ptr<SamplerState>mSampler[SamplerType::max];
	float GaussianDistribution(const VECTOR2F& position, const float rho);
	void CalucurateBluer(const float width, const float hight, const VECTOR2F& dir, const float deviation, const float multiply);
	EditorData mEditorData[4];
	int mNowEditorNo;
	int mNowScene;
};