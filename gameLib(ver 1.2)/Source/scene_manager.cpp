#include "scene_manager.h"
#include"scene_title.h"
#include"scene_game.h"
#include"scene_over.h"
#include"scene_clear.h"
#include"scene_shader_test.h"
#include"key_board.h"


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
	case SCENETYPE::OVER:
		return new SceneOver(mDevice);
		break;
	case SCENETYPE::CLEAR:
		return new SceneClear(mDevice);
		break;
	case SCENETYPE::SHADERTEST:
		return new SceneShaderTest(mDevice);
		break;
	}
	return nullptr;
}
