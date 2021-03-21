#pragma once
#include"scene.h"
#include<memory>
#include"sprite.h"
#include"bloom.h"
#include"blend_state.h"
#include"ranking.h"
#include"fade.h"
#include"sky_map.h"
#include"stage_manager.h"
#include"render_state.h"
#include"sampler_state.h"


class SceneResult :public Scene
{
public:
	SceneResult(float timer,int nowStageNo);
	void Initialize(ID3D11Device* device);
	void Editor();
	void Update(float elapsed_time);
	void Render(ID3D11DeviceContext* context, float elapsed_time);
	void Relese();
private:
	std::unique_ptr<Sprite>mNumberText;
	float mNowGameTime;
	bool mPlayFlag;
	bool nowLoading;
	bool renderFlag;
	std::shared_ptr<FrameBuffer>frameBuffer;
	std::unique_ptr<FrameBuffer>frameBuffer2;
	std::shared_ptr<FrameBuffer>velocityBuffer;
	std::unique_ptr<MulltiRenderTargetFunction>mulltiRenderTarget;
	std::unique_ptr<BloomRender>mBloom;
	std::unique_ptr<Sprite>mRenderScene;
	std::unique_ptr<Ranking>mRanking;
	std::unique_ptr<Fade>mFade;
	std::unique_ptr<SkyMap>sky;
	std::unique_ptr<StageManager>mSManager;
	std::unique_ptr<DrowShader>mMotionBlurShader;
	enum samplerType
	{
		wrap, clamp, max
	};
};
