#pragma once
#include"scene.h"
#include <thread>
#include <mutex>
#include<memory>
#include"sprite.h"
#include"bloom.h"
#include"blend_state.h"
#include"character.h"
#include"model_renderer.h"
#include"static_obj.h"
#include"fade.h"
#include"constant_buffer.h"
#include"zoom_blur_parameter.h"
#include"render_state.h"
#include"sampler_state.h"

class SceneTitle :public Scene
{
public:
	//コンストラクタ
	SceneTitle();
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
	std::unique_ptr<Sprite>test;
	//描画用変数
	std::unique_ptr<BloomRender>bloom;
	std::unique_ptr<FrameBuffer>frameBuffer[3];
	std::unique_ptr<DrowShader>mBluer;
	std::unique_ptr<ConstantBuffer<CbZoom>>mCbZoomBuffer;
	//samplerStateのタイプ
	enum samplerType
	{
		wrap,clamp,max
	};
	//エディターで使う変数
	bool mTestMove;
	//NowLoading時に使う変数
	bool mLoading;
	bool renderFlag;
};