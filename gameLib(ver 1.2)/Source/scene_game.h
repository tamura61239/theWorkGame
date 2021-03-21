#pragma once
#include"scene.h"
#include <thread>
#include <mutex>
#include<memory>
#include"sprite.h"
#include"static_obj.h"
#include"framebuffer.h"
#include"render_effects.h"
#include"model.h"
#include"model_renderer.h"
#include"bloom.h"
#include"player_ai.h"
#include"stage_manager.h"
#include"stage_operation.h"
#include"blend_state.h"
#include"sky_map.h"
#include"drow_shader.h"
#include"motionblur.h"
#include"depth_of_field.h"
#include"stage_select.h"
#include"fade.h"
#include"light_view.h"
#include"tutorial_state.h"
#include"zoom_blur_parameter.h"
#include"constant_buffer.h"
#include"sampler_state.h"
#include"render_state.h"


class SceneGame :public Scene
{
public:
	SceneGame(int stageNo);
	void Initialize(ID3D11Device* device);
	void Editor();
	void Update(float elapsed_time);
	void Render(ID3D11DeviceContext* context, float elapsed_time);
	void Relese();
private:
	//
	std::unique_ptr<Sprite>test;
	std::unique_ptr<Sprite>nowLoading;
	std::unique_ptr<Sprite>siro;
	std::unique_ptr<Sprite>pushKey;
	//デプスバッファ
	std::unique_ptr<FrameBuffer>shadowMap;
	std::shared_ptr<FrameBuffer>frameBuffer;
	std::shared_ptr<FrameBuffer>frameBuffer3;
	std::shared_ptr<FrameBuffer>velocityMap;
	std::shared_ptr<FrameBuffer>shadowRenderBuffer;
	std::shared_ptr<FrameBuffer>frameBuffer2;
	//ゲームを動かすのに必要な変数
	std::shared_ptr<PlayerAI>player;
	std::unique_ptr<StageManager>mSManager;
	std::unique_ptr<StageOperation>mStageOperation;
	std::unique_ptr<TutorialState>mTutorialState;
	//描画に必要な変数
	std::unique_ptr<RenderEffects>mRenderEffects;
	std::unique_ptr<ModelRenderer>mModelRenderer;
	std::unique_ptr<BloomRender>mBloom;
	std::unique_ptr<SkyMap>mSky;
	std::unique_ptr<DrowShader>mBlurShader;
	std::unique_ptr<DrowShader>motionBlurShader;
	std::unique_ptr<DrowShader>skymapMotionShader;
	std::unique_ptr<LightView>mLightView;
	std::unique_ptr<ConstantBuffer<CbZoom>>mCbZoomBuffer;
	enum samplerType
	{
		warp,border, clamp,max
	};
	bool testGame;
	bool hitArea;
	int textureNo;
	bool mNowLoading;
	bool mLoadEnd;
	int mStageNo;
};