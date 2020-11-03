#include "scene_clear.h"
#include"scene_manager.h"
#include"key_board.h"
#ifdef USE_IMGUI
#include <imgui.h>
#include"stage_manager.h"
#endif

SceneClear::SceneClear(ID3D11Device* device):mEditorFlag(true)
{
	test = std::make_unique<Sprite>(device, L"Data/image/クリアテスト.png");
}

void SceneClear::Update(float elapsed_time)
{
	if (ImGuiUpdate())
	{
		return;
	}
}

void SceneClear::Render(ID3D11DeviceContext* context, float elapsed_time)
{
	test->Render(context, VECTOR2F(0, 0), VECTOR2F(222, 96), VECTOR2F(0, 0), VECTOR2F(111, 48), 0);

}

SceneClear::~SceneClear()
{
}

bool SceneClear::ImGuiUpdate()
{
#ifdef USE_IMGUI
	switch (pSceneManager.GetSceneEditor()->Editor(&mEditorFlag, StageManager::GetMaxStageCount()))
	{
	case 1:
		pSceneManager.ChangeScene(SCENETYPE::TITLE);
		return true;
		break;
	case 2:
	case 3:
		pSceneManager.ChangeScene(SCENETYPE::GAME);
		return true;
		break;
	case 4:
		pSceneManager.ChangeScene(SCENETYPE::RESULT);
		return true;
		break;
	}
	if (!mEditorFlag)return false;
#endif
	return false;
}
