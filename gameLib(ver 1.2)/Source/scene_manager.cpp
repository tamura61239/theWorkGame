#include "scene_manager.h"
#include"scene_title.h"
#include"scene_game.h"
#include"scene_result.h"
#include"key_board.h"

#ifdef USE_IMGUI
#include <imgui.h>
#endif



void SceneManager::Update(float elapsed_time)
{
	pKeyBoad.Update();
	mScene->Update(elapsed_time);
}

void SceneManager::Render(ID3D11DeviceContext* context, float elapsed_time)
{
	mScene->Render(context, elapsed_time);
}

void SceneManager::ChangeScene(const int sceneNum)
{
	mScene.reset(SceneSelect(sceneNum));
}

Scene* SceneManager::SceneSelect(const int sceneNum)
{
	switch (sceneNum)
	{
	case SCENETYPE::TITLE:
		return new SceneTitle(mDevice);
		break;
	case SCENETYPE::GAME:
		return new SceneGame(mDevice);
		break;
	case SCENETYPE::RESULT:
		return new SceneResult(mDevice);
		break;
	}
	return nullptr;
}
