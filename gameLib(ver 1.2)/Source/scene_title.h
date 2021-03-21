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
	SceneTitle();
	void Initialize(ID3D11Device* device);
	void Editor();
	void Update(float elapsed_time);
	void Render(ID3D11DeviceContext* context, float elapsed_time);
	void Relese();
	~SceneTitle();
private:
	std::unique_ptr<Sprite>test;
	std::unique_ptr<BloomRender>bloom;
	std::unique_ptr<FrameBuffer>frameBuffer[3];
	std::unique_ptr<DrowShader>mBluer;
	std::unique_ptr<ConstantBuffer<CbZoom>>mCbZoomBuffer;
	enum samplerType
	{
		wrap,clamp,max
	};
	bool mTestMove;
	bool mLoading;
	bool renderFlag;
};