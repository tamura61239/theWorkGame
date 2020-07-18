#include "scene_title.h"
#include"key_board.h"
#include"scene_manager.h"
#ifdef USE_IMGUI
#include<imgui.h>
#endif
SceneTitle::SceneTitle(ID3D11Device* device)
{
	test = std::make_unique<Sprite>(device, L"Data/image/タイトルテスト.png");
}

void SceneTitle::Update(float elapsed_time)
{

	if (pKeyBoad.RisingState(KeyLabel::SPACE))
	{
		pSceneManager.ChangeScene(SCENETYPE::GAME);
		return;
	}
}

void SceneTitle::Render(ID3D11DeviceContext* context, float elapsed_time)
{
	test->Render(context, VECTOR2F(0, 0), VECTOR2F(222, 96), VECTOR2F(0, 0), VECTOR2F(111, 48), 0);
}

SceneTitle::~SceneTitle()
{
}
