#include "scene_over.h"
#include"scene_manager.h"
#include"key_board.h"
SceneOver::SceneOver(ID3D11Device* device)
{
	test = std::make_unique<Sprite>(device, L"Data/image/オーバーテスト.png");
}

void SceneOver::Update(float elapsed_time)
{
	if (pKeyBoad.RisingState(KeyLabel::SPACE))
	{
		pSceneManager.ChangeScene(SCENETYPE::CLEAR);
		return;
	}
}

void SceneOver::Render(ID3D11DeviceContext* context, float elapsed_time)
{
	test->Render(context, VECTOR2F(0, 0), VECTOR2F(222, 96), VECTOR2F(0, 0), VECTOR2F(111, 48), 0);

}

SceneOver::~SceneOver()
{
}
