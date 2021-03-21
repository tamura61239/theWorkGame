#include "scene_select.h"
#include"camera_manager.h"
#include"screen_size.h"
#include"gpu_particle_manager.h"
#include"scene_manager.h"
#include"scene_title.h"
#include"scene_game.h"
#include"scene_result.h"
#ifdef USE_IMGUI
#include<imgui.h>
#endif

/***********************初期化****************************/
void SceneSelect::Initialize(ID3D11Device* device)
{
	//マルチスレッド
	loading_thread = std::make_unique<std::thread>([&](ID3D11Device* device)
		{
			std::lock_guard<std::mutex> lock(loading_mutex);


			mSelect = std::make_unique<StageSelect>(device, StageManager::GetMaxStageCount());
			pCameraManager->Initialize(device, 2);
			pCameraManager->GetCamera()->SetEye(VECTOR3F(0, 0, -200));

			mColorMap = std::make_unique<FrameBuffer>(device, SCREEN_WIDTH, SCREEN_HEIGHT, true, 8, DXGI_FORMAT_R8G8B8A8_UNORM);
			mSceneFrame = std::make_unique<FrameBuffer>(device, SCREEN_WIDTH, SCREEN_HEIGHT, true, 8, DXGI_FORMAT_R8G8B8A8_UNORM);
			GpuParticleManager::Create();

			pGpuParticleManager->CreateSelectBuffer(device);

			mBloom = std::make_unique<BloomRender>(device, static_cast<float>(SCREEN_WIDTH), static_cast<float>(SCREEN_HEIGHT), 1);
		
			mPhotographTargets.resize(2);
			//memset(&mPhotographTargets[0], 0, sizeof(bool) * mPhotographTargets.size());
			mFade = std::make_unique<Fade>(device, Fade::FADE_SCENE::SELECT);
		}, device);
	//NowLoadingの時に描画するもの
	mRenderTexture = std::make_unique<Sprite>(device, L"Data/image/now.png");
	
	mBlend.push_back(std::make_unique<BlendState>(device, BLEND_MODE::ALPHA));
	mBlend.push_back(std::make_unique<BlendState>(device, BLEND_MODE::ADD));

	mSampler.push_back(std::make_unique<SamplerState>(device, D3D11_FILTER_MIN_MAG_MIP_LINEAR, D3D11_TEXTURE_ADDRESS_WRAP));

	mDepth = std::make_unique<DepthStencilState>(device, true, D3D11_DEPTH_WRITE_MASK_ALL, D3D11_COMPARISON_LESS_EQUAL);
	mRasterizer = std::make_unique<RasterizerState>(device, D3D11_FILL_SOLID, D3D11_CULL_NONE, false, true, false, true, false);

}

void SceneSelect::Editor()
{
	if (IsNowLoading())
	{
		return;
	}
	bool beforeEditorFlag = mEditorFlag;
	switch (pSceneManager.GetSceneEditor()->Editor(&mEditorFlag, StageManager::GetMaxStageCount()))
	{
	case SceneManager::SCENETYPE::TITLE:
		pSceneManager.ChangeScene(new SceneTitle);
		break;
	case SceneManager::SCENETYPE::GAME:
		pSceneManager.ChangeScene(new SceneGame(pSceneManager.GetSceneEditor()->GetStageNo()));
		break;
	case SceneManager::SCENETYPE::RESULT:
		pSceneManager.ChangeScene(new SceneResult(0.f,0));
		break;
	}
	if (!mEditorFlag)return;
	ImGui::Begin("scene select");
#ifdef _DEBUG
	if (ImGui::CollapsingHeader("screen shot"))
	{
		ImGui::Selectable("gpu particle", &mPhotographTargets[0]);
		ImGui::Selectable("stage board", &mPhotographTargets[1]);
		if (ImGui::Button("photograph"))
		{
			mScreenShot = true;
		}
	}
#endif
	ImGui::RadioButton("NONE", &mEditorNo, 0);
	ImGui::RadioButton("SELECT_SCENE", &mEditorNo, 1);
	ImGui::RadioButton("BLOOM", &mEditorNo, 2);
	ImGui::RadioButton("CAMERA", &mEditorNo, 3);
	ImGui::RadioButton("FADE", &mEditorNo, 4);
	ImGui::RadioButton("GPU PARTICLE", &mEditorNo, 5);

	ImGui::End();
	switch (mEditorNo)
	{
	case 1:
		mSelect->ImGuiUpdate();
		break;
	case 2:
		mBloom->ImGuiUpdate();
		break;
	case 3:
		pCameraManager->ImGuiUpdate();
		break;
	case 4:
		mFade->ImGuiUpdate();
		break;
	case 5:
		pGpuParticleManager->ImGuiUpdate();
		break;
	}
}
/*******************更新**********************/
void SceneSelect::Update(float elapsed_time)
{
	if (IsNowLoading())
	{
		return;
	}
	EndLoading();

	if (mSelect->Update(elapsed_time))
	{
		mFade->StartFadeOut();
	}
	mFade->Update(elapsed_time);
	pGpuParticleManager->Update(elapsed_time);
	if (mFade->GetEndFlag())
	{
		if (mFade->GetFadeScene() == Fade::FADE_MODO::FADEOUT)
		{
			pSceneManager.ChangeScene(new SceneGame(mSelect->GetSelectNumber()));

		}
	}
	pCameraManager->Update(elapsed_time);
}
/*********************描画******************/
void SceneSelect::Render(ID3D11DeviceContext* context, float elapsed_time)
{
	//描画のためのStateの設定
	mSampler[0]->Activate(context, 0, false, true);
	mDepth->Activate(context);
	mRasterizer->Activate(context);
	mBlend[0]->activate(context);
	//NowLoading中
	if (IsNowLoading())
	{
		mRenderTexture->Render(context, VECTOR2F(1300, 900), VECTOR2F(600, 100), VECTOR2F(0, 0), VECTOR2F(600, 100), 0, LoadColor(elapsed_time));
		mBlend[0]->deactivate(context);
		return;
	}
	/**************カラーマップ*************/
	
	mColorMap->Clear(context);
	mColorMap->Activate(context);


	pGpuParticleManager->Render(context, pCameraManager->GetCamera()->GetView(), pCameraManager->GetCamera()->GetProjection());
	mSelect->Render(context);

	
	mColorMap->Deactivate(context);

	mBlend[0]->deactivate(context);

	/****************ブルーム***************/	
	mBlend[1]->activate(context);
	//ブルームのために縮小テクスチャを作成
	mBloom->BlurTexture(context, mColorMap->GetRenderTargetShaderResourceView().Get());
	//ブルームをかける
	mSceneFrame->Clear(context);
	mSceneFrame->Activate(context);
	mRenderTexture->Render(context, mColorMap->GetRenderTargetShaderResourceView().Get(), VECTOR2F(0, 0), VECTOR2F(1920, 1080), VECTOR2F(0, 0), VECTOR2F(1920, 1080), 0);
	mBloom->Render(context, mColorMap->GetRenderTargetShaderResourceView().Get());
	mSceneFrame->Deactivate(context);
	mBlend[1]->deactivate(context);

	//デプスバッファで描画したものをフロントバッファに描画する
	mBlend[0]->activate(context);

	mRenderTexture->Render(context, mSceneFrame->GetRenderTargetShaderResourceView().Get(), VECTOR2F(0, 0), VECTOR2F(1920, 1080), VECTOR2F(0, 0), VECTOR2F(1920, 1080), 0);

	mFade->Render(context);
	//後処理
	mSampler[0]->DeActivate(context);
	mDepth->DeActive(context);
	mRasterizer->DeActivate(context);
	mBlend[0]->deactivate(context);

}
/***************消去****************/
void SceneSelect::Relese()
{
	pGpuParticleManager->ClearBuffer();
	pCameraManager->DestroyCamera();
}
