#include "scene_result.h"
#include"ui_manager.h"
#include"scene_manager.h"
#include"stage_manager.h"
#ifdef USE_IMGUI
#include<imgui.h>
#endif

SceneResult::SceneResult(ID3D11Device* device) :mNowGameTime(UIManager::GetInctance().GetGameUIMove()->GetTime()), mEditorFlag(false), mEditorNo(0)
{
	UIManager::GetInctance().ResultInitialize(device);
	mBlend.push_back(std::make_unique<blend_state>(device, BLEND_MODE::ALPHA));
	mRanking = std::make_unique<Ranking>(device, mNowGameTime);
	mFade = std::make_unique<Fade>(device, Fade::FADE_SCENE::RESULT);
}

void SceneResult::Update(float elapsed_time)
{
	if (ImGuiUpdate())return;
	if (mFade->GetFadeScene() == Fade::FADE_MODO::FADEOUT)
	{
		if (mFade->GetEndFlag())
		{
			UIManager::GetInctance().Clear();
			mFade->Clear();
			return;
		}
	}
	mRanking->Update(elapsed_time, false);
	UIManager::GetInctance().Update(elapsed_time);
	if (UIManager::GetInctance().GetResultUIMove()->GetDecisionFlag())
	{
		mFade->StartFadeOut();
	}
}

void SceneResult::Render(ID3D11DeviceContext* context, float elapsed_time)
{
	mBlend[0]->activate(context);
	mRanking->Render(context);
	UIManager::GetInctance().Render(context);
	mFade->Render(context);
	mBlend[0]->deactivate(context);
}

SceneResult::~SceneResult()
{
}

bool SceneResult::ImGuiUpdate()
{
#ifdef USE_IMGUI
	switch (pSceneManager.GetSceneEditor()->Editor(&mEditorFlag, StageManager::GetMaxStageCount()))
	{
	case 1:
		UIManager::GetInctance().Clear();
		pSceneManager.ChangeScene(SCENETYPE::TITLE);
		return true;
		break;
	case 2:
	case 3:
		UIManager::GetInctance().Clear();
		pSceneManager.ChangeScene(SCENETYPE::GAME);
		return true;
		break;
	}
	if (!mEditorFlag)return false;
	ImGui::Begin("scene result");
	ImGui::RadioButton("NONE", &mEditorNo, 0);
	ImGui::RadioButton("RANKING", &mEditorNo, 1);
	ImGui::RadioButton("UI", &mEditorNo, 2);
	ImGui::RadioButton("FADE", &mEditorNo, 3);

	ImGui::End();
	switch (mEditorNo)
	{
	case 1:
		mRanking->ImGuiUpdate();
		break;
	case 2:
		UIManager::GetInctance().ImGuiUpdate();
		break;
	case 3:
		mFade->ImGuiUpdate();
		break;

	}
#endif
	return false;
}
