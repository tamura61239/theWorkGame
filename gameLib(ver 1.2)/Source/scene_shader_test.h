#pragma once
#include"scene.h"
#include <thread>
#include <mutex>
#include<memory>
#include"sprite.h"
#include"framebuffer.h"
#include"static_obj.h"
#include<vector>

/****************************シェーダー授業用***********************************/
class SceneShaderTest :public Scene
{
public:
	SceneShaderTest(ID3D11Device* device);
	void Update(float elapsed_time);
	void Render(ID3D11DeviceContext* context, float elapsed_time);
	~SceneShaderTest(){}
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
	std::unique_ptr<Sprite>renderScenes;
	std::shared_ptr<FrameBuffer>colorScene;
	std::shared_ptr<FrameBuffer>positionScene;
	std::shared_ptr<FrameBuffer>normalScene;
	std::unique_ptr<MulltiRenderTargetFunction>mulltiRenderTargets;
	std::vector<std::unique_ptr<StaticObj>>staticObjs;
	std::unique_ptr<MeshRender>render;
};