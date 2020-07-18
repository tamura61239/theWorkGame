#include "scene_clear.h"
#include"scene_manager.h"
#include"key_board.h"
SceneClear::SceneClear(ID3D11Device* device)
{
	test = std::make_unique<Sprite>(device, L"Data/image/クリアテスト.png");
}

void SceneClear::Update(float elapsed_time)
{
	if (pKeyBoad.RisingState(KeyLabel::SPACE))
	{
		pSceneManager.ChangeScene(SCENETYPE::TITLE);
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
