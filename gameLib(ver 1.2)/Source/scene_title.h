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

class SceneTitle :public Scene
{
public:
	SceneTitle(ID3D11Device* device);
	void Update(float elapsed_time);
	void Render(ID3D11DeviceContext* context, float elapsed_time);
	~SceneTitle();
private:
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
private:
	bool ImGuiUpdate();
	std::unique_ptr<Sprite>test;
	std::unique_ptr<BloomRender>bloom;
	std::unique_ptr<FrameBuffer>frameBuffer[2];
	std::unique_ptr<blend_state>blend[2];
	std::unique_ptr<ModelRenderer>modelRender;
	std::unique_ptr<Character>character;
	//std::unique_ptr<ModelRenderer>modelRender;
	//std::unique_ptr<Character>character;
	std::unique_ptr<StaticObj>obj;
	std::unique_ptr<MeshRender>mRender;
	std::unique_ptr<Fade>mFade;
	std::unique_ptr<DrowShader>mBluer;
	bool mEditorFlag;
	bool mTestMove;
};