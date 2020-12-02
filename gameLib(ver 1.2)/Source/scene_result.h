#pragma once
#include"scene.h"
#include <thread>
#include <mutex>
#include<memory>
#include"sprite.h"
#include"bloom.h"
#include"blend_state.h"
#include"ranking.h"
#include"fade.h"
#include"sky_map.h"

class SceneResult :public Scene
{
public:
	SceneResult(ID3D11Device* device);
	void Update(float elapsed_time);
	void Render(ID3D11DeviceContext* context, float elapsed_time);
	~SceneResult();
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
	bool ImGuiUpdate();
private:
	std::unique_ptr<Sprite>mNumberText;
	float mNowGameTime;
	bool mEditorFlag;
	int mEditorNo;
	bool mPlayFlag;
	bool nowLoading;
	std::vector<std::unique_ptr<blend_state>>mBlend;
	std::unique_ptr<FrameBuffer>frameBuffer;
	std::unique_ptr<BloomRender>mBloom;
	std::unique_ptr<Sprite>mRenderScene;
	std::unique_ptr<Ranking>mRanking;
	std::unique_ptr<Fade>mFade;
	std::unique_ptr<SkyMap>sky;

};
