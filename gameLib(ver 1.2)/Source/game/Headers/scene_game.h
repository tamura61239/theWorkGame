#pragma once
#include"scene.h"
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
#include"sky_map.h"
#include"drow_shader.h"
#include"stage_select.h"
#include"light_view.h"
#include"tutorial_state.h"
#include"zoom_blur_parameter.h"
#include"constant_buffer.h"
#include"sampler_state.h"
#include"render_state.h"
#include"motion_blur_parameter.h"


class SceneGame :public Scene
{
public:
	SceneGame(int stageNo);
	//初期化
	void Initialize(ID3D11Device* device);
	//エディター
	void Editor();
	//更新
	void Update(float elapsed_time);
	//描画
	void Render(ID3D11DeviceContext* context, float elapsed_time);
	//解放
	void Relese();
private:
	//フェード時の動き
	void FadeMove(float elapsdTime);
	//画像描画変数
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
	std::unique_ptr<ConstantBuffer<MotionBlurParameter>>mCbMotionBlur;
	//samplerStateのタイプ
	enum samplerType
	{
		warp,border, clamp,max
	};
	//エディター変数
	bool testGame;
	bool hitArea;
	int textureNo;
	//NowLoadingの時に使う変数
	bool mNowLoading;
	bool mLoadEnd;
	//ステージ番号保持変数
	int mStageNo;
};