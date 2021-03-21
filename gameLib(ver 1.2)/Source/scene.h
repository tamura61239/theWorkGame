#pragma once
#include<d3d11.h>
#include <thread>
#include <mutex>
#include"Easing.h"
#include"fade.h"
#include"render_state.h"
#include"sampler_state.h"
#include"blend_state.h"
#include<vector>

class Scene
{
public:
	Scene() = default;
	virtual ~Scene() = default;
	virtual void Initialize(ID3D11Device* device) = 0;
	virtual void Editor() = 0;
	virtual void Update(float elapsed_time) = 0;
	virtual void Render(ID3D11DeviceContext* context, float elapsed_time) = 0;
	virtual void Relese() = 0;
protected:
	//Now Loading
	std::unique_ptr<std::thread> loading_thread;
	std::mutex loading_mutex;


	bool IsNowLoading()
	{
		if (loading_thread && loading_mutex.try_lock())
		{
			loading_mutex.unlock();
			return false;
		}
		return true;
	}
	void EndLoading()
	{
		if (loading_thread && loading_thread->joinable())
		{
			loading_thread->join();
		}
	}
	float mLoadTimer = 0.f;
	//NowLoadingの表示の不透明度計算
	VECTOR4F LoadColor(float elapsdTime,float maxTimer=2)
	{
		mLoadTimer += elapsdTime;
		VECTOR4F color = VECTOR4F(1, 1, 1, 0);
		if (mLoadTimer < maxTimer)
		{
			color.w = Easing::InOutCubic(mLoadTimer, maxTimer);
			return color;
		}
		if (mLoadTimer < maxTimer * 2.f)
		{
			color.w = Easing::InOutCubic(mLoadTimer, maxTimer, 0.f, 1.f);
			return color;
		}
		mLoadTimer = 0;
		return color;
	}
	//エディタ関数用の変数
	int mEditorNo = 0;
	bool mEditorFlag = false;
	float mElapsdTimeSpeed = 1.0f;
	bool mStopTime = false;
	bool mScreenShot = false;
	//スクリーンショットに写すかどうか
	std::vector<bool>mPhotographTargets;
	int mTextureNo = 0;
	//フェードインアウト用変数
	std::unique_ptr<Fade>mFade;
	//描画に必要なstate変数
	std::unique_ptr<DepthStencilState>mDepth;
	std::unique_ptr<RasterizerState>mRasterizer;
	std::vector<std::unique_ptr<SamplerState>>mSampler;
	std::vector<std::unique_ptr<BlendState>>mBlend;

};