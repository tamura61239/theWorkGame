#include "scene_title.h"
#include"key_board.h"
#include"scene_manager.h"
#ifdef USE_IMGUI
#include<imgui.h>
#endif
SceneTitle::SceneTitle(ID3D11Device* device)
{
	test = std::make_unique<Sprite>(device, L"Data/image/タイトルテスト.png");
	testBloom = std::make_unique<BlommTest01>(device, L"Data/image/koko.png");
}

void SceneTitle::Update(float elapsed_time)
{
#ifdef USE_IMGUI
	ImGui::Begin("ChangScene");
	if (ImGui::Button("startShaderTest"))
	{
		pSceneManager.ChangeScene(SCENETYPE::SHADERTEST);
		ImGui::End();
		return;
	}
	ImGui::End();
#endif

	if (pKeyBoad.RisingState(KeyLabel::SPACE))
	{
		pSceneManager.ChangeScene(SCENETYPE::GAME);
		return;
	}
}

void SceneTitle::Render(ID3D11DeviceContext* context, float elapsed_time)
{
	test->Render(context, VECTOR2F(0, 0), VECTOR2F(222, 96), VECTOR2F(0, 0), VECTOR2F(111, 48), 0);
	testBloom->Render(context, VECTOR2F(400, 300), VECTOR2F(225, 224), VECTOR2F(0, 0), VECTOR2F(512, 512), 0);
}

SceneTitle::~SceneTitle()
{
}
