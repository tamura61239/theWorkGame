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

class SceneGame :public Scene
{
public:
	SceneGame(ID3D11Device*device);
	void Update(float elapsed_time);
	void Render(ID3D11DeviceContext* context, float elapsed_time);
	~SceneGame();
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
	std::unique_ptr<Sprite>test;
	std::unique_ptr<Sprite>nowLoading;
	std::unique_ptr<Sprite>siro;
	std::unique_ptr<StaticMesh>mesh;
	std::vector<std::unique_ptr<StaticMesh>>meshs;
	std::vector<std::unique_ptr<StaticObj>>staticObjs;
	std::unique_ptr<MeshRender>meshRender;
	std::unique_ptr<FrameBuffer>shadowMap;
	std::unique_ptr<FrameBuffer>frameBuffer[2];
	std::unique_ptr<RenderEffects>renderEffects;
	std::unique_ptr<PlayerAI>player;
	std::unique_ptr<ModelRenderer>modelRenderer;
	std::unique_ptr<BloomRender>bloom;
	std::unique_ptr<StageManager>mSManager;
	std::unique_ptr<StageOperation>mStageOperation;
};