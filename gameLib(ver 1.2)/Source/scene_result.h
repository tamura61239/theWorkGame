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
	//コンストラクタ
	SceneResult(float timer,int nowStageNo);
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
	//画像描画用変数
	std::unique_ptr<Sprite>mNumberText;
	std::unique_ptr<Sprite>mRenderScene;
	//げーむのクリアタイム
	float mNowGameTime;
	//進行可能フラグ
	bool mPlayFlag;
	//NowLoading時に使う変数
	bool nowLoading;
	bool renderFlag;
	std::unique_ptr<Ranking>mRanking;
	//描画用変数
	std::shared_ptr<FrameBuffer>frameBuffer;
	std::unique_ptr<FrameBuffer>frameBuffer2;
	std::shared_ptr<FrameBuffer>velocityBuffer;
	std::unique_ptr<BloomRender>mBloom;
	std::unique_ptr<Fade>mFade;
	std::unique_ptr<SkyMap>sky;
	//samplerStateのタイプ
	enum samplerType
	{
		wrap, clamp, max
	};
};
