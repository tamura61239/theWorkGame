#include "scene_manager.h"
#include"scene_title.h"
#include"scene_game.h"
#include"scene_result.h"
#include"key_board.h"

#ifdef USE_IMGUI
#include <imgui.h>
#endif



void SceneManager::Initialize(ID3D11Device* device)
{
	mScene = std::make_unique<SceneTitle>();
	mDevice = device;
	mScene->Initialize(device);
}

void SceneManager::Update(float elapsed_time)
{
	if (mNextScene.get() != nullptr)
	{
		if(mScene!=nullptr)mScene->Relese();
		mScene.reset();
		mNextScene->Initialize(mDevice);
		mScene = std::move(mNextScene);
		//mNextScene.reset();
	}
	pKeyBoad.Update();
	mScene->Editor();
	mScene->Update(elapsed_time);
}

void SceneManager::Render(ID3D11DeviceContext* context, float elapsed_time)
{
	mScene->Render(context, elapsed_time);
}

void SceneManager::ChangeScene(Scene*scene)
{
	mNextScene.reset(scene);
}

Scene* SceneManager::SceneSelect(const int sceneNum)
{
	//switch (sceneNum)
	//{
	//case SCENETYPE::TITLE:
	//	return new SceneTitle(mDevice);
	//	break;
	//case SCENETYPE::GAME:
	//	return new SceneGame(mDevice);
	//	break;
	//case SCENETYPE::RESULT:
	//	return new SceneResult(mDevice);
	//	break;
	//}
	return nullptr;
}
