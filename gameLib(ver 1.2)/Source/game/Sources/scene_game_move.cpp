#include "..\Headers\scene_game_move.h"
#include"ui_manager.h"
#include"scene_manager.h"
#include"scene_result.h"
#include"camera_manager.h"
#include"file_function.h"

#ifdef USE_IMGUI
#include<imgui.h>
#endif
/*****************************************************/
//　　　　　　　　　　初期化関数(コンストラクタ)
/*****************************************************/
SceneGameMove::SceneGameMove(ID3D11Device* device, int stageNo):mStageNo(stageNo)
{
	//チュートリアルクラス生成
	mTutorial = std::make_unique<TutorialState>(device, mStageNo);
	//フェードクラス生成
	mFade = std::make_unique<Fade>(device, Fade::FADE_SCENE::GAME);
	mFade->StartFadeIn();
	//ズームブラーパラメータークラス生成
	mCbZoomBlurBuffer = std::make_unique<ConstantBuffer<CbZoom>>(device);
	FileFunction::Load(mCbZoomBlurBuffer->data, "Data/file/game_zoom_blur_parameter.bin", "rb");
	//シェーダークラス生成
	D3D11_INPUT_ELEMENT_DESC input_element_desc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }

	};
	mZoomBlurShader = std::make_unique<DrowShader>(device, "Data/shader/sprite_vs.cso", "", "Data/shader/zoom_blur_ps.cso", input_element_desc, ARRAYSIZE(input_element_desc));

}
/*****************************************************/
//　　　　　　　　　　エディタ関数
/*****************************************************/
void SceneGameMove::ZoomBlurEditor()
{
	//ズームブラーのパラメーターを操作する
	ImGui::Begin("zoom blur");
	ImGui::InputFloat("length", &mCbZoomBlurBuffer->data.lenght, 0.1f);
	ImGui::InputInt("division", &mCbZoomBlurBuffer->data.division, 1);
	//セーブ
	if (ImGui::Button("save"))
	{
		FileFunction::Save(mCbZoomBlurBuffer->data, "Data/file/game_zoom_blur_parameter.bin", "wb");
	}
	ImGui::End();

}
/*****************************************************/
//　　　　　　　　　　更新関数
/*****************************************************/
float SceneGameMove::Update(float elapsdTime, PlayerManager* player, bool testGame, bool editorFlag)
{
	//elapsdTime *= mTutorial->Update(elapsdTime, player->GetCharacter());
	mFade->Update(elapsdTime);
	float time = 0.0f;
	switch (mFade->GetFadeScene())
	{
	case Fade::FADE_MODO::NONE://ゲーム中
		time = SceneMove(elapsdTime,player,testGame);
		break;
	case Fade::FADE_MODO::FADEIN://シーンに入ったばかり
		time = SceneStart(elapsdTime, editorFlag);
		break;
	case Fade::FADE_MODO::FADEOUT://ゴール後
		time = SceneEnd(elapsdTime);
		break;
	}
	mGorlFlag = player->GetCharacter()->GetGorlFlag();
	return time;
}
/********************ゴール後の関数***********************/
float SceneGameMove::SceneEnd(float elapsdTime)
{
	if (mFade->GetEndFlag())
	{
		pSceneManager.ChangeScene(new SceneResult(UIManager::GetInctance()->GetGameUIMove()->GetTime(), mStageNo));
	}
	return elapsdTime * 0.3f;
}
/********************シーンに入ったばかりの関数***********************/
float SceneGameMove::SceneStart(float elapsdTime, bool editorFlag)
{
	if (mFade->GetEndFlag())
	{
		mFade->Clear();
		if (!editorFlag)UIManager::GetInctance()->GetGameUIMove()->Start();
	}
	return elapsdTime;
}
/********************ゲーム中の関数***********************/
float SceneGameMove::SceneMove(float elapsdTime, PlayerManager* player, bool testGame)
{
	UIManager::GetInctance()->Update(elapsdTime);
	if (player->GetPlayFlag())
	{
		//プレイ中にタイムが0になるかゴールした時
		if (UIManager::GetInctance()->GetGameUIMove()->GetTime() <= 0 || player->GetCharacter()->GetGorlFlag())
		{
			if (!testGame && mFade->GetFadeScene() == Fade::FADE_MODO::NONE)
			{
				mFade->StartFadeOut();
			}
			return elapsdTime*0.3f;
		}
	}
	else
	{
		//スタートのカウントが0になった時
		if (UIManager::GetInctance()->GetGameUIMove()->GetCount() <= 0)
		{
			if (UIManager::GetInctance()->GetGameUIMove()->GetStartFlag())
			{
				player->SetPlayFlag(true);
				pCameraManager->GetCameraOperation()->GetPlayCamera()->SetStartPosition(player->GetCharacter()->GetPosition());

			}
		}
	}
	return elapsdTime;
}
/*****************************************************/
//　　　　　　　　　　描画関数
/*****************************************************/

void SceneGameMove::Render(ID3D11DeviceContext* context, ID3D11ShaderResourceView* srv, Sprite* render)
{
	if (mGorlFlag)
	{//ゴール後
		mCbZoomBlurBuffer->Activate(context, 0, true, true);
		render->Render(context, mZoomBlurShader.get(), srv, VECTOR2F(0, 0), VECTOR2F(1920, 1080), VECTOR2F(0, 0), VECTOR2F(1920, 1080), 0);
		mCbZoomBlurBuffer->DeActivate(context);
	}
	else if (mTutorial->GetState() == 1 || mTutorial->GetState() == 3)
	{//チュートリアルの特定の状態
		mTutorial->GetCbZoom()->Activate(context, 0, true, true);
		render->Render(context, mZoomBlurShader.get(), srv, VECTOR2F(0, 0), VECTOR2F(1920, 1080), VECTOR2F(0, 0), VECTOR2F(1920, 1080), 0, mTutorial->GetBackGroundColor());
		mTutorial->GetCbZoom()->DeActivate(context);

	}
	else
	{//その他
		render->Render(context, srv, VECTOR2F(0, 0), VECTOR2F(1920, 1080), VECTOR2F(0, 0), VECTOR2F(1920, 1080), 0);
	}
	mFade->Render(context);

}

